#version 400 core

/* ==============================================================================
        Stage Inputs
 ==============================================================================*/
layout(location = 0) in vec3 Position;

/* ==============================================================================
        Uniforms
 ==============================================================================*/
uniform cameraBlock {
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

/* ==============================================================================
        Stage Outputs
 ==============================================================================*/
out vec3 worldPos;

void main() {
    worldPos = Position;

	mat4 rotView = mat4(mat3(ViewMatrix));
	vec4 clipPos = ProjMatrix * rotView * vec4(worldPos, 1.0);

	gl_Position = clipPos.xyww;
}