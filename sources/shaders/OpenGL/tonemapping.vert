#version 450 core

layout(location = 0) in vec4 VertexPosition;

void main() {
	// Place the verted in the screen
	gl_Position = VertexPosition;
}