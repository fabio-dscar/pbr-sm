#version 450 core

// Imports
vec3 simpleToneMap(vec3 c, float exp);
vec3 unchartedTonemap(vec3 c, float exp);
vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C,
                           float D, float E, float J, float W);
vec3 toInverseGamma(vec3 c, float gamma);

in vec3 worldPos;

layout(std140, binding = 1) uniform rendererBlock {
    float gamma;
    float exposure;

    float A, B, C, D, E, J, W;
};

layout(location = 1) uniform samplerCube envMap;

out vec4 outColor;

void main() {
    vec3 envColor = textureLod(envMap, worldPos, 2.0).rgb;

    envColor = unchartedTonemapParam(envColor, exposure, A, B, C, D, E, J, W);
    envColor = toInverseGamma(envColor, gamma);

    outColor = vec4(envColor, 1.0);
}
