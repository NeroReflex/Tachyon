#version 450

layout (location = 0) out vec4 FragColor;

in vec2 UV;

uniform float gamma;
uniform float exposure;

// Values that stay constant for the whole mesh.
layout (binding = 0) uniform sampler2D outputSampler;

void main() {
	// base pixel colour for image
	vec3 hdrColor = texture(outputSampler, UV).rgb;

	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// Apply gamma correction and output the result
	FragColor = vec4(pow(mapped, vec3(1.0 / gamma)), 1.0);
}