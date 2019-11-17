#version 450 core

#include "../config.glsl"

layout (location = 0) out vec4 FragColor;

// Values that stay constant for the whole mesh.
layout (binding = OUTPUT_BINDING) uniform sampler2D outputSampler;

void main() {
	// Apply gamma correction and output the result
	FragColor = vec4(texture(outputSampler, vec2(gl_FragCoord.xy) / vec2(textureSize(outputSampler, 0))).rgb, 1.0);
}