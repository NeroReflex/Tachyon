#version 450

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 VertexTextureCoord;

out vec2 UV;

void main() {
	// Write out UV coordinates
	UV = VertexTextureCoord;

	// Place the verted in the screen
	gl_Position = vec4(VertexPosition, 0, 0);
}