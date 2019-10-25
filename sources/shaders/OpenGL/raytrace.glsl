#version 450

// This gets adjusted on each call by the OpenGL core
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

uniform uint width;
uniform uint height;

/*
buffer ModelMatrices
{
	mat4 Model[];
};
*/

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	// Avoid calculating useless pixels
	if ((gl_GlobalInvocationID.x >= width) || (gl_GlobalInvocationID.y >= height))
		return;

	//
	// interesting stuff happens here later
	//
	pixel = vec4(1, 1, 1, 1.0);


	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}