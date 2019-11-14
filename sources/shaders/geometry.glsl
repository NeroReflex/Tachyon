#ifndef GEOMETRY_GLSL
#define GEOMETRY_GLSL

/**
 * This represents a sphere.
 */
struct Geometry {
	vec3 center;

	float radius;
};

Geometry emptyGeometry = Geometry(vec3(0, 0, 0), 0.0);

#endif // GEOMETRY_GLSL
