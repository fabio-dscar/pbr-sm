#version 450 core

const float PI = 3.14159265358979;

vec3 toLinearRGB(vec3 c, float gamma) { return pow(c, vec3(gamma)); }
vec3 toInverseGamma(vec3 c, float gamma) { return pow(c, vec3(1.0 / gamma)); }

vec3 unchartedTonemap(vec3 c, float exp) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;

    c = exp * c;
    return ((c * (A * c + C * B) + D * E) / (c * (A * c + B) + D * F)) - E / F;
}

vec3 unchartedTonemap(vec3 v, float A, float B, float C, float D, float E,
                      float J) {
    return ((v * (A * v + C * B) + D * E) / (v * (A * v + B) + D * J)) - E / J;
}

vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C,
                           float D, float E, float J, float W) {
    vec3 scale = unchartedTonemap(vec3(W), A, B, C, D, E, J);
    vec3 ret = unchartedTonemap(exp * c, A, B, C, D, E, J);
    return ret / scale;
}

float luminance(vec3 c) {
    vec3 RGBtoY = vec3(0.2126, 0.7152, 0.0722);
    return dot(c, RGBtoY);
}

// [Schlick, 1994] - "An Inexpensive BRDF Model for Physically-based Rendering"
float fresnelSchlick(float cosTheta) {
    float A = clamp(1.0 - cosTheta, 0, 1);
    return pow(A, 5);
}

vec3 fresnelSchlickUnreal(float cosTheta, vec3 F0) {
    // return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0 - F0) * exp2(cosTheta * (-5.55473 * cosTheta - 6.98316));
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --------------------------------------------------------------------------

float brdfLambert() { return 1.0 / PI; }

/* ============================================================================
        Microfacet Models
 ============================================================================*/

// [Walter et al, 2007] - "Microfacet Models for Refraction through Rough
// Surfaces"
float distGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / (denom + 0.00001);
}

float distGGX_Sphere(vec3 N, vec3 H, float roughness, float dist, float radius) {
    float a = roughness * roughness;
    float a2 = a * a;
    float ap = clamp(a + radius / (2 * dist), 0.0, 1.0);
    float ap2 = ap * ap;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);

    return (a2 * ap2) / (denom * denom + 0.0000001);
}

// [Walter et al. 2007, "Microfacet models for refraction through rough
// surfaces"]
float distBeckmann(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH2 = NdotH * NdotH;

    return exp((NdotH2 - 1) / (a2 * NdotH2)) / (PI * a2 * NdotH2 * NdotH2);
}

// Geometric attenuation for *analytic* light sources
float geoGGX(float NdotV, float roughness) {
    // Remap roughness like [Burley, 2012]
    float r = (roughness + 1.0) / 2.0;
    float k = (r * r) / 2.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Geometric attenuation for environment map lighting
float geoGGX_IBL(float NdotV, float roughness) {
    float a = roughness;
    float k = (a * a) / 2.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float geoSmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    float GGX1 = geoGGX(NdotV, roughness);
    float GGX2 = geoGGX(NdotL, roughness);

    return GGX1 * GGX2;
}
