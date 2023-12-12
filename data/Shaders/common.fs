const float PI = 3.14159265358979;
const float MaxSpecularLod = 8;
const float ClearCoatF0 = 0.04;

const int NUM_LIGHTS = 5;
const int LIGHT_NONE = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;
const int LIGHT_DIR = 3;
const int LIGHT_SPHERE = 4;
const int LIGHT_TUBE = 5;

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
        Microfacet Model
 ============================================================================*/

float distGGX(float NdotH, float rough) {
    float a2 = rough * rough;
    float NdotH2 = NdotH * NdotH;
    float denom = NdotH2 * (a2 - 1) + 1.0;
    return a2 / (PI * denom * denom);
}

float visSmithGGX(float NdotL, float NdotV, float rough) {
    float a2 = rough * rough;
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;

    float geoL = NdotV * sqrt(NdotL2 * (1.0 - a2) + a2);
    float geoV = NdotL * sqrt(NdotV2 * (1.0 - a2) + a2);
    float geoSum = geoL + geoV;

    if (geoSum > 0.0)
        return 0.5 / geoSum;

    return 0.0;
}

vec3 fresnSchlick(float HdotV, vec3 F0) {
    float p = pow(1.0 - HdotV, 5.0);
    return F0 * (1.0 - p) + p;
}

float fresnSchlick(float HdotV, float F0, float F90) {
    return F0 + (F90 - F0) * pow(1.0 - HdotV, 5.0);
}