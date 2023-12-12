#include <common.fs>

in vec3 worldPos;

layout(location = 1) uniform samplerCube envMap;

out vec4 outColor;

void main() {
    vec3 envColor = texture(envMap, worldPos).rgb;

    envColor = ToneMap(toneMapType, envColor, exposure);
    envColor = toInverseGamma(envColor, gamma);

    outColor = vec4(envColor, 1.0);
}
