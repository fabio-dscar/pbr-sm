#version 400

// Math constants
const float PI = 3.14159265358979;

/* ==============================================================================
        Utils
 ============================================================================== */

// From inverse gamma space to linear
vec3 toLinearRGB(vec3 c, float gamma) {
    return pow(c, vec3(gamma));
}

// From linear to inverse gamma space
vec3 toInverseGamma(vec3 c, float gamma) {
    return pow(c, vec3(1.0 / gamma));
}

vec3 simpleToneMap(vec3 c, float exp) {
	vec3 color = exp * c;
	return color / (color + vec3(1.0));
}

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

vec3 unchartedTonemap(vec3 v, float A, float B, float C, float D, float E, float J) {
	return ((v * (A * v + C * B) + D * E) / (v * (A * v + B) + D * J)) - E / J;
}

vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C, float D, float E, float J, float W) {
	vec3 scale = unchartedTonemap(vec3(W), A, B, C, D, E, J);
	vec3 ret   = unchartedTonemap(exp * c, A, B, C, D, E, J);
	return ret / scale;
}

// Convert to Y component of XYZ color space
float luminance(vec3 c) {
    vec3 RGBtoY = vec3(0.2126, 0.7152, 0.0722);
    return dot(c, RGBtoY);
}

// [Schlick, 1994] - "An Inexpensive BRDF Model for Physically-based Rendering"
float fresnelSchlick(float cosTheta){
    float A = clamp(1.0 - cosTheta, 0, 1);
    return pow(A, 5);
}

vec3 fresnelSchlickUnreal(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// --------------------------------------------------------------------------

/* ============================================================================
        Diffuse BRDFs
 ============================================================================ */

float brdfLambert() {
    return 1.0 / PI;
}

// [Burley, 2012] - "Physically-Based Shading at Disney"
float brdfBurley(in float roughness, in float NdotV, in float NdotL, in float VdotH) {
	float FD90 = 0.5 + 2 * VdotH * VdotH * roughness;
    float FV = fresnelSchlick(NdotV);
    float FL = fresnelSchlick(NdotL);
    float FD = mix(1.0, FD90, FL) * mix(1.0, FD90, FV);

	return (1.0 / PI) * FD;
}

// [Gotanda, 2012] - "Beyond a Simple Physically Based Blinn-Phong Model in Real-Time"
// -> Modern Oren Nayar approximation developed for PS3/XBOX360 consoles
float brdfOrenNayarTriAce() {
	return 0.0f;
}

/* ============================================================================
        Specular BRDFs
 ============================================================================*/

float specMicrofacet(in float HdotL, in float HdotR, in float D, in float G) {
    if (HdotL == 0 || HdotR == 0)
        return 0;

    float F = fresnelSchlick(HdotL);
    return (D * G * F) / (4.0 * HdotL * HdotR);
}

/* ============================================================================
        Microfacet Models
 ============================================================================*/

 // [Walter et al, 2007] - "Microfacet Models for Refraction through Rough Surfaces"
float distGGX(vec3 N, vec3 H, float roughness) {
    float a  = roughness * roughness;
    float a2 = a * a;

    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float distBeckmann(float NdotH, float roughness) {
	float a  = roughness * roughness;
	float a2 = a * a;

	float NdotH2 = NdotH * NdotH;

	return exp((NdotH2 - 1) / (a2 * NdotH2)) / (PI * a2 * NdotH2 * NdotH2);
}

// Geometric attenuation function for *analytic* light sources
float geoGGX(float NdotV, float roughness) {
    // Remap roughness like [Burley, 2012]
    float r = (roughness + 1.0) / 2.0;
    float k = (r * r) / 2.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Geometric attenuation function for environment map lighting
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

/* ============================================================================
        BSSRDFs
 ============================================================================*/

// Based on Hanrahan-Krueger's brdf approximation of isotropic bssrdf
// [Burley, 2012] - "Physically-Based Shading at Disney"
float bssrdfHKIsotropic(float NdotL, float NdotV, float LdotH, float roughness) {
    float FL = fresnelSchlick(NdotL);
    float FV = fresnelSchlick(NdotV);

    float Fss90 = LdotH * LdotH * roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);

    return 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);
}