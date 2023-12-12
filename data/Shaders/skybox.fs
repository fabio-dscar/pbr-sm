#include <common.fs>

in vec3 worldPos;

layout(std140, binding = 1) uniform rendererBlock {
    float gamma;
    float exposure;

    // Tone map params
    float A, B, C, D, E, J, W;

    float envIntensity;
    int perturbNormals;
    int envLighting;
};

layout(location = 1) uniform samplerCube envMap;

out vec4 outColor;

void main() {
    vec3 envColor = textureLod(envMap, worldPos).rgb;

    envColor = unchartedTonemapParam(envColor, exposure, A, B, C, D, E, J, W);
    envColor = toInverseGamma(envColor, gamma);

    outColor = vec4(envColor, 1.0);
}
