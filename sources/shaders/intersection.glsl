#ifndef INTERSECTION_GLSL
#define INTERSECTION_GLSL

#include "utils.glsl"
#include "ray.glsl"
#include "geometry.glsl"

struct RayGeometryIntersection {
	float dist;
	vec4 point;
	vec4 normal;
};

const RayGeometryIntersection miss = RayGeometryIntersection(infinity, vec4(0, 0, 0, 1), vec4(0, 0, 0, 0));

RayGeometryIntersection bestHit(const in RayGeometryIntersection isect1, const in RayGeometryIntersection isect2) {
	return (isect1.dist <= isect2.dist) ? isect1 : isect2;
}

RayGeometryIntersection intersectGeometry(const in Ray ray, const in Geometry geometry, const in mat4 transformMatrix, const in float minDistance, const in float maxDistance) {
	const vec3 center = vec3(transformMatrix * vec4(geometry.center, 1));
	const float radius = geometry.radius;

	if (radius == 0) return miss;

	const vec3 origin = vec3(ray.origin);
	const vec3 direction = vec3(ray.direction);

	vec3 oc = origin - center;

	const float a = dot(direction, direction);
	const float b = dot(oc, direction);
	const float c = dot(oc, oc) - radius * radius;
	const float discriminant = b * b - a * c;

	if (discriminant > 0.0) { // if delta > 0 then we have two intersections (one for each side of the sphere)
		const float squareRoot = sqrt(discriminant);

		// x0 and x1 are the distances to the origin of the ray
		float x0 = (-b - squareRoot) / a, x1 = (-b + squareRoot) / a;

		// Use x0 and x1 to calculate intersection points
		vec3 point_x0 = rayPointAt(ray, x0), point_x1 = rayPointAt(ray, x1); // so if I use that distance as a coefficient I obtain the intersection point

		// Use intersecting points to calculate the surface normal at that point
		vec3 normal_x0 = (point_x0 - center) / radius, normal_x1 = (point_x1 - center) / radius; // and the normal for a point p is (point - center)/radius

		// No valid intersection? return a miss.
		if (((x0 <= minDistance) || (x0 >= maxDistance)) && ((x1 <= minDistance) || (x1 >= maxDistance))) {
			return miss;
		}

		// Choose the closest intersection point
		return (((x0 > minDistance) && (x0 < maxDistance)) && (x0 <= x1)) || ((x1 <= minDistance) || (x1 >= maxDistance)) ?
			RayGeometryIntersection(x0, vec4(point_x0, 1), vec4(normal_x0, 0)) : RayGeometryIntersection(x1, vec4(point_x1, 1), vec4(normal_x1, 0));
	}

	return miss;
}

RayGeometryIntersection intersectCollection_ByIndexes(const in Ray ray, const in uint blasIndex, const in uint collectionIndex, const in mat4 transformMatrix, const in float minDistance, const in float maxDistance) {
	RayGeometryIntersection bestHitSoFar = intersectGeometry(ray, ReadGeometry_ByIndexes(blasIndex, collectionIndex, 0), transformMatrix, minDistance, maxDistance);

	for (uint i = 1; i < (1 << expOfTwo_maxGeometryOnCollection); ++i) {
		// Check if this is a better hit than the former one
		bestHitSoFar = bestHit(
			bestHitSoFar,
			intersectGeometry(ray, ReadGeometry_ByIndexes(blasIndex, collectionIndex, i), transformMatrix, minDistance, maxDistance) // Compare to the result of ray-geometry intersection algorithm
		);
	}

	return bestHitSoFar;
}

/**
 * Check if the ray has missed every geometry that has been tested against.
 *
 * @param test the ray-geometry intersection algorithm output
 * @return TRUE if the ray has missed all geometry
 */
bool hasMissed(const in RayGeometryIntersection test) {
	return isinf(test.dist) || isnan(test.dist);
}

RayGeometryIntersection intersectBLAS_ByIndex(const in Ray ray, const in uint blasIndex, const in float minDistance, const in float maxDistance) {
	// Adjust transformation matrix to consider the BLAS model matrix
	const mat4 ModelMatrix = ReadModelMatrix_ByIndex(blasIndex);

	RayGeometryIntersection bestHitSoFar = miss;

	int currentDepth = 0;
	uint currentPath = 0x0000;

	while ((currentDepth < (expOfTwo_maxCollectionsForModel + 1)) && (currentDepth >= 0)) {
		// This for cycles is used to reach the interesting node index (the result is in currentNodeIndex)
		uint currentNodeIndex = 0;
		for (uint i = 0; i < currentDepth; ++i) {
			currentNodeIndex = ((currentPath & (1 << currentDepth)) == 0) ? leftNode(currentNodeIndex) : rightNode(currentNodeIndex);
		}

		bool goDeeper = false;

		if (intersectAABB(ray, ReadAABBFromBLAS_ByIndexes(blasIndex, currentNodeIndex), ModelMatrix)) {
			//imageStore(debugOutput, ivec2(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y)), vec4(0,0,1,1));
			if (isBLASNodeLeaf_ByIndex(currentNodeIndex)) {
				bestHitSoFar = bestHit( // Update the best hit so far by comparing...
					bestHitSoFar, // the bast hit so far
					intersectCollection_ByIndexes(ray, blasIndex, LeafFromBLASNode_ByIndex(currentNodeIndex), ModelMatrix, minDistance, maxDistance) // and the result of the ray-collection traverse
				);
			}
			else {
				// Go one level deeper
				goDeeper = true;
				currentDepth += 1;
			}
		}

		if (!goDeeper) {
			// in either case: subtree discarded or visited leaf the algorithm should reduce the depth and change the path
			currentDepth -= 1;

			while ((currentPath & (1 << currentDepth)) == 1) {
				//set the bit at currentDepth position to 0
				currentPath &= ~(1 << currentDepth);

				if (currentDepth == 0) return bestHitSoFar;

				// and decrease currentDepth
				currentDepth -= 1;
			}

			// set the bit at currentDepth position to 1 (the next visit will choose another path)
			currentPath |= (1 << currentDepth);
		}
	}

	return bestHitSoFar;
}

RayGeometryIntersection castRay(const in Ray ray, const in float minDistance, const in float maxDistance) {
	RayGeometryIntersection bestHitSoFar = miss;

	uint currentDepth = 0;
	uint currentPath = 0;

	// Traverse the entire TLAS to find the best intersection
	while (currentDepth <= expOfTwo_maxModels) {
		// This for cycles is used to reach the interesting node index (the result is in currentNodeIndex)
		uint currentNodeIndex = 0;
		for (uint i = 0; i < currentDepth; ++i) {
			currentNodeIndex = ((currentPath & (1 << currentDepth)) == 0) ? leftNode(currentNodeIndex) : rightNode(currentNodeIndex);
		}

		bool goDeeper = false;

		if (intersectAABB(ray, ReadAABBFromTLAS_ByIndex(currentNodeIndex), identityTransform)) {
			if (isTLASNodeLeaf_ByIndex(currentNodeIndex)) {
				bestHitSoFar = bestHit( // Update the best hit so far by comparing...
					bestHitSoFar, // the bast hit so far
					intersectBLAS_ByIndex(ray, LeafFromTLASNode_ByIndex(currentNodeIndex), minDistance, maxDistance) // and the result of the ray-BLAS traverse
				);
			}
			else {
				// Go one level deeper
				goDeeper = true;
				currentDepth += 1;
			}
		}

		if (!goDeeper) {
			// in either case: subtree discarded or visited leaf the algorithm should reduce the depth and change the path
			currentDepth -= 1;

			while ((currentPath & (1 << currentDepth)) == 1) {
				//set the bit at currentDepth position to 0
				currentPath &= ~(1 << currentDepth);

				if (currentDepth == 0) return bestHitSoFar;

				// and decrease currentDepth
				currentDepth -= 1;
			}

			// set the bit at currentDepth position to 1 (the next visit will choose another path)
			currentPath |= (1 << currentDepth);
		}
	}

	return bestHitSoFar;
}

#endif // INTERSECTION_GLSL
