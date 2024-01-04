#include <common.fs>

// -----------------------------------------------------------------------------
//    Stage Inputs
// -----------------------------------------------------------------------------
// Everything in world coordinates
in FragData {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
}
vsIn;

// -----------------------------------------------------------------------------
//    Uniforms
// -----------------------------------------------------------------------------
layout(std140, binding = 1) uniform cameraBlock {
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

layout(std140, binding = 2) uniform lightBlock { Light lights[NUM_LIGHTS]; };

// Material parameters
layout(location = 1) uniform sampler2D diffuseTex;
layout(location = 2) uniform sampler2D normalTex;
layout(location = 3) uniform sampler2D metallicTex;
layout(location = 4) uniform sampler2D roughTex;
layout(location = 5) uniform sampler2D aoTex;
layout(location = 6) uniform sampler2D emissiveTex;
layout(location = 7) uniform sampler2D clearCoatNormTex;

layout(location = 8) uniform vec3 diffuse;
layout(location = 9) uniform float metallic;
layout(location = 10) uniform float roughness;
layout(location = 11) uniform float spec;
layout(location = 12) uniform float clearCoatRough;
layout(location = 13) uniform float clearCoat;

// IBL precomputation
layout(location = 15) uniform samplerCube irradianceTex;
layout(location = 16) uniform samplerCube ggxTex;
layout(location = 17) uniform sampler2D brdfTex;

out vec4 outColor;

struct ShadingContext {
    vec3 V, N, R, L, H;
    vec3 kd, F0, Li, Le;
    vec3 clearCoatNormal;
    float NdotV, NdotL;
    float a, ao, att, specNorm, metal, rough, dist;
};

vec3 PerturbNormal(in sampler2D normalMap) {
    vec3 normal = texture(normalMap, vsIn.texCoords).rgb;
    normal = normal * 2.0 - 1.0;
    return normalize(vsIn.TBN * normal);
}

// Closest point to sphere
vec3 closestPtSphere(inout ShadingContext sc, float radius) {
    vec3 centerToRay = dot(sc.L, sc.R) * sc.R - sc.L;
    vec3 closest = sc.L + centerToRay * clamp(radius / length(centerToRay), 0.0, 1.0);
    sc.dist = length(sc.L);
    return closest;
}

// [Karis, 2013] "Real Shading in Unreal Engine 4"
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

    sc.NdotL = (2.0 * clamp(NdotL0 + NdotL1, 0.0, 1.0)) / (distL0 * distL1 + dot(L0, L1) + 2.0);
    sc.dist = length(closest);

    return closest;
}

float PointAttenuation(float dist, float invRadius) {
    float distSq = dist * dist;
    float factor = distSq * invRadius * invRadius;
    float smoothFactor = max(1.0 - factor * factor, 0.0);
    return smoothFactor * smoothFactor / max(distSq, 1e-4);
}

float SpotAngleAttenuation(vec3 L, vec3 spotDir, float cosInner, float cosOuter) {
    float theta = dot(L, normalize(-spotDir));
    float f = max(cosInner - cosOuter, 1e-4);
    float att = clamp((theta - cosOuter) / f, 0.0, 1.0);
    return att * att;
}

void CalcLightAttrs(inout ShadingContext sc, in Light l) {
    sc.Li = l.emission;

    vec3 toLight = l.position - vsIn.position;
    if (l.type == LIGHT_DIR)
        toLight = -l.position;

    sc.L = toLight;
    sc.dist = length(sc.L);
    if (l.type == LIGHT_SPHERE)
        sc.L = closestPtSphere(sc, l.auxA);
    else if (l.type == LIGHT_TUBE)
        sc.L = closestPtTube(sc, l, toLight);

    sc.L = normalize(sc.L);
    sc.H = normalize(sc.V + sc.L);

    // Light attenuation
    sc.att = 1.0;
    if (l.type == LIGHT_SPOT)
        sc.att = SpotAngleAttenuation(sc.L, l.auxB, l.auxA, l.auxC);
    sc.att *= PointAttenuation(sc.dist, 0.01);

    // Normalization factor
    // Only specular component for area lights
    sc.specNorm = 1.0;
    switch (l.type) {
        case LIGHT_SPHERE:
        case LIGHT_TUBE:
            float radius = l.auxA;
            float ap = clamp(sc.a + radius / (2.0 * sc.dist), 0.0, 1.0);
            sc.specNorm = ap * ap * PI;
            break;
    }
}

vec3 EvalSpecularIBL(vec3 R, float NdotV, vec3 F0, float roughness) {
    vec3 cubeConv = textureLod(ggxTex, R, roughness * MaxSpecularLod).rgb;
    vec2 brdf = texture(brdfTex, vec2(NdotV, roughness)).rg;

#ifdef MULTISCATTERING
    vec3 iblSpecular = mix(brdf.xxx, brdf.yyy, F0) * cubeConv;
    vec3 multiscattering = 1.0 + F0 * (1.0 / brdf.y - 1.0);
    iblSpecular *= multiscattering;
    return iblSpecular;
#else
    return (F0 * brdf.r + brdf.g) * cubeConv;
#endif
}

vec3 EnvironmentLighting(in ShadingContext sc) {
    if (envLighting == 0)
        return vec3(0);

    vec3 diffuseColor = (1.0 - sc.metal) * sc.kd;
    vec3 irradiance = texture(irradianceTex, sc.N).rgb;

    vec3 iblDiffuse = diffuseColor * irradiance * brdfLambert();
    vec3 iblSpecular = EvalSpecularIBL(sc.R, sc.NdotV, sc.F0, sc.rough);

#ifdef HAS_CLEARCOAT
    // ---------------------------------------------------------------------
    //    Clearcoat
    // ---------------------------------------------------------------------
    float Fcc = fresnSchlick(sc.NdotV, ClearCoatF0, 1.0) * clearCoat;
    vec3 iblClearCoat = EvalSpecularIBL(sc.R, sc.NdotV, vec3(ClearCoatF0), clearCoatRough);

    // Attenuate base layer
    iblDiffuse *= 1.0 - Fcc;
    iblSpecular *= 1.0 - Fcc;

    return (iblDiffuse + iblSpecular) * sc.ao + iblClearCoat * clearCoat;
#else
    return (iblDiffuse + iblSpecular) * sc.ao;
#endif
}

vec3 ShadingLight(in ShadingContext sc) {
    float HdotV = clamp(dot(sc.H, sc.V), 0.0, 1.0);
    float NdotH = clamp(dot(sc.N, sc.H), 0.0, 1.0);

    float D = distGGX(NdotH, sc.a);
    float V = visSmithGGX(sc.NdotL, sc.NdotV, sc.a);
    vec3 F = fresnSchlick(HdotV, sc.F0);

    vec3 specular = D * V * F;
    vec3 diffuse = (1.0 - sc.metal) * sc.kd * brdfLambert();
    vec3 baseLayer = (1.0 - F) * diffuse + specular;

    vec3 Li = sc.Li * sc.att;

#ifdef HAS_CLEARCOAT
    // ---------------------------------------------------------------------
    //    Clearcoat
    // ---------------------------------------------------------------------
    float HdotL = clamp(dot(sc.H, sc.L), 0.0, 1.0);

    float remapClearRough = clamp(clearCoatRough, 0.089, 1.0);
    float clearCoatA = remapClearRough * remapClearRough;

    float Dcc = distGGX(NdotH, clearCoatA);
    float Vcc = visKelemen(HdotL);
    float Fcc = fresnSchlick(HdotV, ClearCoatF0, 1.0) * clearCoat;
    float clearCoatLayer = Dcc * Vcc * Fcc;

    return (baseLayer * (1.0 - Fcc) + clearCoatLayer) * Li * sc.NdotL;
#else
    return baseLayer * Li * sc.NdotL;
#endif
}

void GetMaterial(inout ShadingContext sc) {
    sc.kd = diffuse * toLinearRGB(texture(diffuseTex, vsIn.texCoords).rgb, gamma);
    sc.rough = roughness * texture(roughTex, vsIn.texCoords).r;
    sc.rough = clamp(sc.rough, 0.089, 1.0);
    sc.metal = metallic * texture(metallicTex, vsIn.texCoords).r;
    sc.ao = texture(aoTex, vsIn.texCoords).r;
    sc.a = sc.rough * sc.rough;
    sc.Le = toLinearRGB(texture(emissiveTex, vsIn.texCoords).rgb, gamma);

#ifdef HAS_CLEARCOAT
    sc.clearCoatNormal = PerturbNormal(clearCoatNormTex);
#endif

    // Map F0 to diffuse color for metals and maxmimum 0.04 for dielectrics
    sc.F0 = 0.16 * spec * spec * (1.0 - sc.metal) + sc.kd * sc.metal;
}

void main(void) {
    ShadingContext sc;

    sc.V = normalize(ViewPos - vsIn.position);
    sc.N = PerturbNormal(normalTex);
    sc.R = reflect(-sc.V, sc.N);

    GetMaterial(sc);

    sc.NdotV = clamp(dot(sc.N, sc.V), 0.0, 1.0);

    vec3 Lenv = EnvironmentLighting(sc) * envIntensity;

    vec3 Lrad = vec3(0);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        const Light l = lights[i];
        if (l.type == LIGHT_NONE)
            continue;

        CalcLightAttrs(sc, l);

        if (l.type != LIGHT_TUBE)
            sc.NdotL = max(dot(sc.N, sc.L), 0.0);

        Lrad += ShadingLight(sc);
    }

    vec3 Lsum = sc.Le + Lenv + Lrad;
    Lsum = ToneMap(toneMapType, Lsum, exposure);
    Lsum = toInverseGamma(Lsum, gamma);

    outColor = vec4(Lsum, 1);
}