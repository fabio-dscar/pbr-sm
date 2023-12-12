#version 450 core

// Imports
vec3 toLinearRGB(vec3 c, float gamma);
vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C,
                           float D, float E, float J, float W);
vec3 toInverseGamma(vec3 c, float gamma);
vec3 fresnelSchlickUnreal(float cosTheta, vec3 F0);
float fresnelSchlick(float cosTheta);
float fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

float geoGGX(float NdotV, float roughness);
float geoSmith(vec3 N, vec3 V, vec3 L, float roughness);
float distGGX(vec3 N, vec3 H, float roughness);
float distGGX_Sphere(vec3 N, vec3 H, float roughness, float dist, float radius);

const float PI = 3.14159265358979;
const float MAX_GGX_LOD = 4.0;

const int NUM_LIGHTS = 5;
const int LIGHT_NONE = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;
const int LIGHT_DIR = 3;
const int LIGHT_SPHERE = 4;

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

layout(location = 5) uniform vec3 diffuse;
layout(location = 6) uniform float metallic;
layout(location = 7) uniform float roughness;
layout(location = 8) uniform vec3 spec;

// IBL precomputation
layout(location = 15) uniform samplerCube irradianceTex;
layout(location = 16) uniform samplerCube ggxTex;
layout(location = 17) uniform sampler2D brdfTex;

out vec4 outColor;

// Cope with these instead of linking conditionally
float FetchParameter(sampler2D samp, float val) {
    if (val >= 0.0)
        return val;
    else
        return texture(samp, vsIn.texCoords).r;
}

vec3 FetchDiffuse() {
    if (diffuse.r >= 0)
        return diffuse;
    else
        return toLinearRGB(texture(diffuseTex, vsIn.texCoords).rgb, gamma);
}

vec3 CalcNormalFragTBN(in sampler2D normalMap) {
    vec3 normal = texture(normalMap, vsIn.texCoords).xyz * 2.0 - 1.0;

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

// Closest point to sphere
vec3 closestPtSphere(vec3 L, vec3 R, float radius) {
    vec3 centerToRay = dot(L, R) * R - L;
    vec3 closest = L + centerToRay * clamp(radius / length(centerToRay), 0.0, 1.0);
    return closest;
}

struct ShadingContext {
    vec3 V, N, R, L, H;
    vec3 kd, F0, Li;
    float NdotV;
    float att, specNorm, metal, rough;
};

void CalcLightAttrs(inout ShadingContext sc, in Light l) {
    vec3 toLight = l.position - vsIn.position;
    if (l.type == LIGHT_DIR)
        toLight = -l.position;

    vec3 L = toLight;
    float dist = 1.0;
    if (l.type == LIGHT_SPHERE) {
        dist = length(L);
        L = closestPtSphere(L, sc.R, l.auxA);
    }

    sc.L = normalize(L);
    sc.H = normalize(sc.V + sc.L);
    sc.Li = l.emission;

    // Light attenuation
    sc.att = 1.0;
    switch (l.type) {
        case LIGHT_SPOT:
            float theta = dot(sc.L, normalize(-l.auxB));
            float eps = l.auxA - l.auxC;
            sc.att = clamp((theta - l.auxC) / eps, 0.0, 1.0);
        case LIGHT_POINT:
            dist = length(toLight);
        case LIGHT_SPHERE:
            sc.att *= 1.0 / (dist * dist);
            break;
    }

    // Normalization factor
    // Only specular component for area lights
    sc.specNorm = 1;
    if (l.type == LIGHT_SPHERE) {
        float radius = l.auxA;
        float a = sc.rough * sc.rough;
        float ap = clamp(a + radius / (2 * dist), 0, 1);
        sc.specNorm = ap * ap * PI;
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
    vec3 prefGGX = textureLod(ggxTex, sc.R, sc.rough * MAX_GGX_LOD).rgb;
    vec3 brdf = texture(brdfTex, vec2(sc.NdotV, sc.rough)).rgb;

    vec3 brdfInt = sc.F0 * brdf.r + brdf.g;
    vec3 specular = prefGGX * brdfInt;

    return (1.0 - F) * (1.0 - sc.metal) * diffuse + specular;
}

vec3 ShadingLight(in ShadingContext sc) {
    float HdotV = max(dot(sc.H, sc.V), 0.0);
    float NdotL = max(dot(sc.N, sc.L), 0.0);

    float fGGX = distGGX(sc.N, sc.H, sc.rough);
    float Geo = geoSmith(sc.N, sc.V, sc.L, sc.rough);
    vec3 Fr = fresnelSchlickUnreal(HdotV, sc.F0);

    vec3 specular = (fGGX * Geo * Fr) / (4.0 * sc.NdotV * NdotL + 0.0001);
    vec3 diff = sc.kd / PI;

    vec3 Li = sc.Li * sc.att;

    return ((vec3(1.0) - Fr) * (1.0 - sc.metal) * diff + specular * sc.specNorm) * Li * NdotL;
}

void main(void) {
    ShadingContext sc;

    sc.V = normalize(ViewPos - vsIn.position);

    sc.N = vec3(0.0);
    if (perturbNormals == 1)
        sc.N = PerturbNormal(normalTex);
    else
        sc.N = CalcNormalFragTBN(normalTex);

    sc.R = reflect(-sc.V, sc.N);

    sc.kd = FetchDiffuse();
    sc.rough = FetchParameter(roughTex, roughness);
    sc.metal = FetchParameter(metallicTex, metallic);

    sc.NdotV = max(dot(sc.N, sc.V), 0.0);

    vec3 Lenv = EnvironmentLighting(sc) * envIntensity;

    vec3 Lrad = vec3(0.0);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        const Light l = lights[i];

        if (l.type == LIGHT_NONE)
            continue;

        CalcLightAttrs(sc, l);

        Lrad += ShadingLight(sc);
    }

    vec3 Lsum = Lenv + Lrad;
    Lsum = unchartedTonemapParam(Lsum, exposure, A, B, C, D, E, J, W);
    Lsum = toInverseGamma(Lsum, gamma);

    outColor = vec4(Lsum, 1.0);
}