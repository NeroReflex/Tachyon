#ifndef AABB_GLSL
#define AABB_GLSL

#include "geometry.glsl"

/**
 * This represents an Axis-Aligned Bounding Box.
 *
 *    +--------------+ < Max vertex (max x, y and z amongs all vertices)
 *   /|             /|
 *  / |            / |
 * *--+-----------*  |
 * |  |           |  | Depth
 * |  |           |  |
 * |  |           |  |
 * |  +-----------+--+
 * | /            | /
 * |/             |/ Width
 * *--------------*
 * ^    Length
 * Position vertex (minimum x, y and z amongs all vertices)
 */
struct AABB {
	/**
	 * This is the position vertex of the AABB.
	 *
	 * The used format is vec4(x, y, z, 1)
	 */
	vec4 position; // this is the point of the aabb with min x, y and z components

	/**
	 * This is the collection of AABB dimensions.
	 *
	 * The used format is vec4(Length, Depth, Width, 0)
	 * Note: fastest way to find the max vertex of the AABB: position+dimensions
	 */
	vec4 dimensions; // this is the amount to be added to aabbVertex to obtain the point with max x, y and z in the aabb
};

/**
 * Represents an empty AABB (that has no area nor volume).
 */
AABB emptyAABB = AABB(vec4(0, 0, 0, 1), vec4(0, 0, 0, 0));

/**
 * Check if the AABB is empty.
 *
 * @param aabb the AABB to be checked
 * @return TRUE iif the AABB is a point in space
 */
bool isEmpty(const in AABB aabb) {
	return (
		((aabb.dimensions.x == 0) || (aabb.dimensions.y == 0) || (aabb.dimensions.z == 0)) ||
		(isinf(aabb.dimensions.x) || isinf(aabb.dimensions.y) || isinf(aabb.dimensions.z)) ||
		(isnan(aabb.dimensions.x) || isnan(aabb.dimensions.y) || isnan(aabb.dimensions.z))
		);
}

/**
 * Calculate the area of the given AABB.
 *
 * @param aabb the AABB to use for area calculation
 * @return the area of the AABB
 */
float getAreaAABB(const in AABB aabb) {
	return 2 * ((aabb.dimensions.x * aabb.dimensions.z) + (aabb.dimensions.x * aabb.dimensions.y) + (aabb.dimensions.y * aabb.dimensions.z));
}

float SAH(const in AABB containedAABB, const in AABB containerAABB) {
	return getAreaAABB(containedAABB) / getAreaAABB(containerAABB);
}

/**
 * Apply a transform to the given AABB.
 *
 * @param starting the AABB transformation will be applied to
 * @param transformMatrix the transform to be applied to all vertices of the AABB, given as a 4x4 matrix
 * @return the transformed AABB
 */
AABB transformAABB(const in AABB b, const in mat4 transformMatrix) {
	vec4 v[8] = {
		transformMatrix * vec4(b.position.x                 , b.position.y                 , b.position.z                 , 1),
		transformMatrix * vec4(b.position.x                 , b.position.y                 , b.position.z + b.dimensions.z, 1),
		transformMatrix * vec4(b.position.x                 , b.position.y + b.dimensions.y, b.position.z                 , 1),
		transformMatrix * vec4(b.position.x                 , b.position.y + b.dimensions.y, b.position.z + b.dimensions.z, 1),
		transformMatrix * vec4(b.position.x + b.dimensions.x, b.position.y                 , b.position.z                 , 1),
		transformMatrix * vec4(b.position.x + b.dimensions.x, b.position.y                 , b.position.z + b.dimensions.z, 1),
		transformMatrix * vec4(b.position.x + b.dimensions.x, b.position.y + b.dimensions.y, b.position.z                 , 1),
		transformMatrix * vec4(b.position.x + b.dimensions.x, b.position.y + b.dimensions.y, b.position.z + b.dimensions.z, 1),
	};

	float maxX = v[0].x, maxY = v[0].y, maxZ = v[0].z, minX = v[0].x, minY = v[0].y, minZ = v[0].z;

	for (uint i = 1; i < 8; ++i) {
		maxX = (v[i].x > maxX) ? v[i].x : maxX;
		maxY = (v[i].y > maxY) ? v[i].y : maxY;
		maxZ = (v[i].z > maxZ) ? v[i].z : maxZ;
		minX = (v[i].x < minX) ? v[i].x : minX;
		minY = (v[i].y < minY) ? v[i].y : minY;
		minZ = (v[i].z < minZ) ? v[i].z : minZ;
	}

	return AABB(vec4(minX, minY, minZ, 1), vec4(maxX - minX, maxY - minY, maxZ - minZ, 0));
}

/**
 * Test intersaction of the given ray with the ginve AABB after applying a transformation to the AABB.
 *
 * @param ray the input ray
 * @param aabb the input AABB
 * @param transformMatrix the transformation to be applied to the input AABB
 * @return TRUE iif the ray intersects the transformed AABB
 */
bool intersectAABB(const in Ray ray, const in AABB aabb, const in mat4 transformMatrix) {
	AABB transformedAABB = transformAABB(aabb, transformMatrix);

	if (isEmpty(transformedAABB)) return false;

	const vec3 invRaydir = getInvDirection(ray);

	const vec3 t0 = ((transformedAABB.position).xyz - ray.origin.xyz) * invRaydir;
	const vec3 t1 = ((transformedAABB.position + transformedAABB.dimensions).xyz - ray.origin.xyz) * invRaydir;
	const vec3 tmin = min(t0, t1), tmax = max(t0, t1);
	return max_component(tmin) <= min_component(tmax);
}

/**
 * Create the union of two AABBs.
 *
 * @param aabb1 the first AABB
 * @param aabb2 the second AABB
 * @return the union of the two AABBs
 */
AABB joinAABBs(const in AABB aabb1, const in AABB aabb2) {
	if (isEmpty(aabb1)) return aabb2;
	if (isEmpty(aabb2)) return aabb1;

	vec4 vMin = vec4(
		min(aabb1.position.x, aabb2.position.x),
		min(aabb1.position.y, aabb2.position.y),
		min(aabb1.position.y, aabb2.position.y),
		1);

	vec4 vMax = vec4(
		max(aabb1.position.x + aabb1.dimensions.x, aabb2.position.x + aabb2.dimensions.x),
		max(aabb1.position.y + aabb1.dimensions.y, aabb2.position.y + aabb2.dimensions.y),
		max(aabb1.position.z + aabb1.dimensions.z, aabb2.position.z + aabb2.dimensions.z),
		1);

	return AABB(vMin, vMax - vMin);
}

AABB generateFromGeometry(const in Geometry geo) {
	if (geo.radius <= 0) return emptyAABB;

	const float diameter = 2 * geo.radius;
	return AABB(vec4(geo.center.x - geo.radius, geo.center.y - geo.radius, geo.center.z - geo.radius, 1), vec4(diameter, diameter, diameter, 0));
}

AABB expandAABBWithGeometry(const in AABB aabb, const in Geometry geo) {
	AABB geometryAABB = generateFromGeometry(geo);
	return joinAABBs(aabb, geometryAABB);
}

#endif // GEOMETRY_GLSL
