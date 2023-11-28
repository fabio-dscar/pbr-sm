#version 450 core

// Imports
vec3 toLinearRGB(vec3 c, float gamma);
vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C,
                           float D, float E, float J, float W);
vec3 toInverseGamma(vec3 c, float gamma);
vec3 fresnelSchlickUnreal(float cosTheta, vec3 F0);
float fresnelSchlick(float cosTheta);

float geoGGX(float NdotV, float roughness);
float geoSmith(vec3 N, vec3 V, vec3 L, float roughness);
float distGGX(vec3 N, vec3 H, float roughness);
float distGGX_Sphere(vec3 N, vec3 H, float roughness, float dist, float radius);

const float PI = 3.14159265358979;
const float MAX_GGX_LOD = 4;

const int NUM_LIGHTS = 5;
const int LIGHT_NONE = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;
const int LIGHT_DIR = 3;
const int LIGHT_SPHERE = 4;
const int LIGHT_TUBE = 5;

/* ==============================================================================
        Stage Inputs
 ==============================================================================*/
// Everything in world coordinates
in FragData {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
}
vsIn;

/* ==============================================================================
        Uniforms
 ==============================================================================*/
layout(std140, binding = 1) uniform rendererBlock {
    float gamma;
    float exposure;

    // Tone map params
    float A, B, C, D, E, J, W;

    float envIntensity;
    int perturbNormals;
    int envLighting;
};

layout(std140, binding = 2) uniform cameraBlock {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
    mat4 ViewProjMatrix;
    vec3 ViewPos;
};

struct Light {
    vec3 position;
    float auxA;
    vec3 emission;
    int type;
    vec3 auxB;
    float auxC;
};

layout(std140, binding = 3) uniform lightBlock { Light lights[NUM_LIGHTS]; };

// Material parameters
layout(location = 1) uniform sampler2D diffuseTex;
layout(location = 2) uniform sampler2D normalTex;
layout(location = 3) uniform sampler2D metallicTex;
layout(location = 4) uniform sampler2D roughTex;
layout(location = 5) uniform sampler2D aoTex;
layout(location = 6) uniform sampler2D emissiveTex;

layout(location = 7) uniform vec3 diffuse;
layout(location = 8) uniform float metallic;
layout(location = 9) uniform float roughness;
layout(location = 10) uniform vec3 spec;

// IBL precomputation
layout(location = 15) uniform samplerCube irradianceTex;
layout(location = 16) uniform samplerCube ggxTex;
layout(location = 17) uniform sampler2D brdfTex;

out vec4 outColor;

vec3 CalcNormalFragTBN(in sampler2D normalMap) {
    vec3 normal = texture(normalMap, vsIn.texCoords).xyz * 2 - 1;

    vec2 duvdx = dFdx(vsIn.texCoords);
    vec2 duvdy = dFdy(vsIn.texCoords);
    vec3 dpdx = dFdx(vsIn.position);
    vec3 dpdy = dFdy(vsIn.position);

    // Find tangent from derivates and build the TBN basis
    vec3 N = normalize(vsIn.normal);
    vec3 T = normalize(dpdx * duvdy.t - dpdy * duvdx.t);
    vec3 B = -normalize(cross(N, T));

    return normalize(mat3(T, B, N) * normal);
}

vec3 PerturbNormal(in sampler2D normalMap) {
    vec3 normal = texture(normalMap, vsIn.texCoords).rgb;
    normal = normal * 2.0 - 1.0;
    return normalize(vsIn.TBN * normal);
}

struct ShadingContext {
    vec3 V, N, R, L, H;
    vec3 kd, F0, Li;
    float NdotV, NdotL;
    float ao, att, specNorm, metal, rough;
};

// Closest point to sphere
vec3 closestPtSphere(vec3 L, vec3 R, float radius) {
    vec3 centerToRay = dot(L, R) * R - L;
    vec3 closest = L + centerToRay * clamp(radius / length(centerToRay), 0, 1);
    return closest;
}

vec3 closestPtTube(inout ShadingContext sc, in Light l, vec3 L) {
    vec3 L0 = L;
    vec3 L1 = l.auxB - vsIn.position;
    float distL0 = length(L0);
    float distL1 = length(L1);
    float NdotL0 = dot(L0, sc.N) / (2 * distL0);
    float NdotL1 = dot(L1, sc.N) / (2 * distL0);

    vec3 Ld = L1 - L0;
    float RdotL0 = dot(sc.R, L0);
    float RdotLd = dot(sc.R, Ld);
    float L0dotLd = dot(L0, Ld);
    float distLd = length(Ld);
    float t = (RdotL0 * RdotLd - L0dotLd) / (distLd * distLd - RdotLd * RdotLd);

    vec3 closest = L0 + Ld * clamp(t, 0, 1);
    vec3 centerToRay = dot(closest, sc.R) * sc.R - closest;
    closest = closest + centerToRay * clamp(l.auxA / length(centerToRay), 0, 1);

    sc.NdotL = (2 * clamp(NdotL0 + NdotL1, 0, 1)) / (distL0 * distL1 + dot(L0, L1) + 2);

    return closest;
}

void CalcLightAttrs(inout ShadingContext sc, in Light l) {
    sc.Li = l.emission;

    vec3 toLight = l.position - vsIn.position;
    if (l.type == LIGHT_DIR)
        toLight = -l.position;

    vec3 L = toLight;
    float dist = 1;
    if (l.type == LIGHT_SPHERE) {
        dist = length(L);
        L = closestPtSphere(L, sc.R, l.auxA);
    } else if (l.type == LIGHT_TUBE) {
        L = closestPtTube(sc, l, toLight);
        dist = length(L);
    }

    sc.L = normalize(L);
    sc.H = normalize(sc.V + sc.L);

    // Light attenuation
    sc.att = 1;
    switch (l.type) {
        case LIGHT_SPOT:
            float theta = dot(sc.L, normalize(-l.auxB));
            float eps = l.auxA - l.auxC;
            sc.att = clamp((theta - l.auxC) / eps, 0, 1);
        case LIGHT_POINT:
            dist = length(toLight);
        case LIGHT_SPHERE:
        case LIGHT_TUBE:
            sc.att *= 1 / (dist * dist);
            break;
    }

    // Normalization factor
    // Only specular component for area lights
    sc.specNorm = 1;
    switch (l.type) {
        case LIGHT_SPHERE:
        case LIGHT_TUBE:
            float radius = l.auxA;
            float a = sc.rough * sc.rough;
            float ap = clamp(a + radius / (2 * dist), 0, 1);
            sc.specNorm = ap * ap * PI;
            break;
    }
}

vec3 EnvironmentLighting(inout ShadingContext sc) {
    if (envLighting == 0) {
        sc.F0 = mix(spec, sc.kd, sc.metal);
        return vec3(0);
    }

    // Diffuse
    vec3 irradiance = texture(irradianceTex, sc.N).rgb;
    vec3 diffuse = sc.kd * irradiance;

    // Specular
    sc.F0 = mix(spec, sc.kd, sc.metal);
    vec3 F = fresnelSchlickUnreal(sc.NdotV, spec);

    // Precomputed integrals
    vec3 prefGGX = textureLod(ggxTex, sc.R, sc.rough * (MAX_GGX_LOD - 1)).rgb;
    vec3 brdf = texture(brdfTex, vec2(sc.NdotV, sc.rough)).rgb;

    vec3 brdfInt = sc.F0 * brdf.r + brdf.g;
    vec3 specular = prefGGX * brdfInt;

    return ((1 - F) * (1 - sc.metal) * diffuse + specular) * sc.ao;
}

vec3 ShadingLight(in ShadingContext sc) {
    float HdotV = max(dot(sc.H, sc.V), 0);

    float fGGX = distGGX(sc.N, sc.H, sc.rough);
    float Geo = geoSmith(sc.N, sc.V, sc.L, sc.rough);
    vec3 Fr = fresnelSchlickUnreal(HdotV, sc.F0);

    vec3 specular = (fGGX * Geo * Fr) / (4 * sc.NdotV * sc.NdotL + 0.0001);
    vec3 diff = sc.kd / PI;

    vec3 Li = sc.Li * sc.att;

    return ((1 - Fr) * (1 - sc.metal) * diff + specular * sc.specNorm) * Li * sc.NdotL;
}

void main(void) {
    ShadingContext sc;

    sc.V = normalize(ViewPos - vsIn.position);
    sc.N = PerturbNormal(normalTex);
    sc.R = reflect(-sc.V, sc.N);

    sc.kd = diffuse * toLinearRGB(texture(diffuseTex, vsIn.texCoords).rgb, gamma);
    sc.rough = roughness * texture(roughTex, vsIn.texCoords).r;
    sc.metal = metallic * texture(metallicTex, vsIn.texCoords).r;
    sc.ao = texture(aoTex, vsIn.texCoords).r;

    vec3 emissive = texture(emissiveTex, vsIn.texCoords).rgb;

    sc.NdotV = max(dot(sc.N, sc.V), 0);

    vec3 Lenv = EnvironmentLighting(sc) * envIntensity;

    vec3 Lrad = vec3(0);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        const Light l = lights[i];

        if (l.type == LIGHT_NONE)
            continue;

        CalcLightAttrs(sc, l);

        if (l.type != LIGHT_TUBE)
            sc.NdotL = max(dot(sc.N, sc.L), 0);

        Lrad += ShadingLight(sc);
    }

    vec3 Lsum = emissive + Lenv + Lrad;
    Lsum = unchartedTonemapParam(Lsum, exposure, A, B, C, D, E, J, W);
    Lsum = toInverseGamma(Lsum, gamma);

    outColor = vec4(Lsum, 1);
}