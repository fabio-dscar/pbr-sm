#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec4 Tangent;

layout(location = 20) uniform mat4 ModelMatrix;
layout(location = 21) uniform mat3 NormalMatrix;

layout(std140, binding = 2) uniform cameraBlock {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
    mat4 ViewProjMatrix;
    vec3 ViewPos;
};

// Everything in world coordinates
out FragData {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
}
vsOut;

void main(void) {
    vsOut.position = vec3(ModelMatrix * vec4(Position, 1.0));
    vsOut.normal = normalize(NormalMatrix * Normal);
    vsOut.texCoords = TexCoords;

    vec3 T = normalize(vec3(ModelMatrix * vec4(Tangent.xyz, 0.0)));
    vec3 N = normalize(vec3(ModelMatrix * vec4(Normal, 0.0)));
    vec3 B = Tangent.w * normalize(cross(N, T));
    vsOut.TBN = mat3(T, B, N);

    gl_Position = ViewProjMatrix * vec4(vsOut.position, 1.0);
}