#version 400

/* ==============================================================================
        Stage Inputs
 ==============================================================================*/
in vec3 worldPos;

/* ==============================================================================
        Uniforms
 ==============================================================================*/
uniform rendererBlock {
	float gamma;
	float exposure;

	// Uncharted tone map parameters (see common.fs)
	float A, B, C, D, E, J, W;
};

uniform samplerCube envMap;

/* ==============================================================================
        Imports
 ==============================================================================*/
vec3 simpleToneMap(vec3 c, float exp);
vec3 unchartedTonemap(vec3 c, float exp);
vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C, float D, float E, float J, float W);
vec3 toInverseGamma(vec3 c, float gamma);

/* ==============================================================================
        Stage Outputs
 ==============================================================================*/
out vec4 outColor;

void main() {		
    vec3 envColor = textureLod(envMap, worldPos, 2.0).rgb;
    
    // Apply tonemap and gamma correction
    envColor = unchartedTonemapParam(envColor, exposure, A, B, C, D, E, J, W);
	envColor = toInverseGamma(envColor, gamma);

    outColor = vec4(envColor, 1.0);
}
