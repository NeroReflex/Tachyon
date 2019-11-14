#ifndef RAY_GLSL
#define RAY_GLSL

struct Ray {
	vec4 origin;
	vec4 direction;
};

vec3 rayPointAt(const in Ray ray, const in float coeff) {
	return vec3(ray.origin) + coeff * vec3(ray.direction);
}

vec3 getInvDirection(const in Ray ray) {
	vec3 ray_dir = ray.direction.xyz;

	/* // Sum/subtract an epsilon to avoid using infinite value
	ray_dir.x += (ray_dir.x >= 0) ? 0.001 : -0.001;
	ray_dir.y += (ray_dir.z >= 0) ? 0.001 : -0.001;
	ray_dir.z += (ray_dir.z >= 0) ? 0.001 : -0.001;
	*/

	return normalize(1.0 / (ray_dir));
}

#endif // RAY_GLSL