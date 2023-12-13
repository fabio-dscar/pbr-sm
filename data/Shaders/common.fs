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

const int TONEMAP_PARAM = 0;
const int TONEMAP_ACES = 1;
const int TONEMAP_ACES_BOOSTED = 2;
const int TONEMAP_ACES_FAST = 3;

layout(std140, binding = 1) uniform rendererBlock {
    float gamma;
    float exposure;
    int toneMapType;

    // Tone map params
    float A, B, C, D, E, J, W;

    float envIntensity;
    int perturbNormals;
    int envLighting;
};

vec3 toLinearRGB(vec3 c, float gamma) { return pow(c, vec3(gamma)); }
vec3 toInverseGamma(vec3 c, float gamma) { return pow(c, vec3(1.0 / gamma)); }

vec3 ParametricToneMap(vec3 v, float A, float B, float C, float D, float E,
                       float J) {
    return ((v * (A * v + C * B) + D * E) / (v * (A * v + B) + D * J)) - E / J;
}

vec3 ParametricToneMap(vec3 c, float A, float B, float C,
                       float D, float E, float J, float W) {
    vec3 scale = ParametricToneMap(vec3(W), A, B, C, D, E, J);
    vec3 ret = ParametricToneMap(c, A, B, C, D, E, J);
    return ret / scale;
}

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESNarkowicz(vec3 color) {
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    return clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0);
}

// Approximation from BakingLab:
// https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
const mat3 ACESInputMat = mat3(
    0.59719, 0.07600, 0.02840,
    0.35458, 0.90834, 0.13383,
    0.04823, 0.01566, 0.83777);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
const mat3 ACESOutputMat = mat3(
    1.60475, -0.10208, -0.00327,
    -0.53108, 1.10813, -0.07276,
    -0.07367, -0.00605, 1.07602);

vec3 RRTAndODTFit(vec3 color) {
    vec3 a = color * (color + 0.0245786) - 0.000090537;
    vec3 b = color * (0.983729 * color + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESHill(vec3 color) {
    color = ACESInputMat * color;
    color = RRTAndODTFit(color);
    color = ACESOutputMat * color;
    return clamp(color, 0.0, 1.0);
}

vec3 ToneMap(int type, vec3 color, float exposure) {
    color *= exposure;

    switch (type) {
        case TONEMAP_PARAM:
            return ParametricToneMap(color, A, B, C, D, E, J, W);
        case TONEMAP_ACES_FAST:
            return ACESNarkowicz(color);
        case TONEMAP_ACES_BOOSTED:
            color /= 0.6;
        case TONEMAP_ACES:
            return ACESHill(color);
        default:
            return vec3(1.0, 0.0, 0.0);
    };
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

float visKelemen(float HdotL) {
    return 0.25 / (HdotL * HdotL);
}

vec3 fresnSchlick(float HdotV, vec3 F0) {
    float p = pow(1.0 - HdotV, 5.0);
    return F0 * (1.0 - p) + p;
}

float fresnSchlick(float HdotV, float F0, float F90) {
    return F0 + (F90 - F0) * pow(1.0 - HdotV, 5.0);
}