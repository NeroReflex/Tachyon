#ifndef CORE_GLSL
#define CORE_GLSL

#include "utils.glsl"
#include "geometry.glsl"
#include "aabb.glsl"
#include "bvh.glsl"

layout(rgba32f, binding = 0) uniform TLAS_MEMORY_MODEL image1D tlas; // Raytracing Top-Level Acceleration Structure: X is the node index

layout(rgba32f, binding = 1) uniform BLAS_MOEMORY_MODEL image2D tlasBLAS; // This is the BLAS collection: X is the node index, Y is the referred BLAS

layout(rgba32f, binding = 2) uniform GEOMETRY_MEMORY_MODEL image3D globalGeometry; // This is the BLAS collection: X is the geometry index, Y is the referred geometry collection (BLAS leaf), Z is the referred BLAS

layout(rgba32f, binding = 3) uniform MODELMATRIX_MEMORY_MODEL image2D ModelMatrix; // This is the collection of Model Matrices for each BLAS

mat4 ReadModelMatrix_ByIndex(const in uint index) {
	return mat4(
		imageLoad(ModelMatrix, ivec2(0, index)),
		imageLoad(ModelMatrix, ivec2(1, index)),
		imageLoad(ModelMatrix, ivec2(2, index)),
		imageLoad(ModelMatrix, ivec2(3, index))
	);
}

#if !defined(MODELMATRIX_READONLY)
void WriteModelMatrix_ByIndex(const in uint index, const in mat4 matrix) {
	imageStore(ModelMatrix, ivec2(0, index), matrix[0]);
	imageStore(ModelMatrix, ivec2(1, index), matrix[1]);
	imageStore(ModelMatrix, ivec2(2, index), matrix[2]);
	imageStore(ModelMatrix, ivec2(3, index), matrix[3]);
}
#endif

Geometry ReadGeometry_ByIndexes(const in uint blasIndex, const in uint collectionIndex, const in uint indexOnCollection) {
	vec4 center = imageLoad(globalGeometry, ivec3((indexOnCollection * (1 << expOfTwo_numOfVec4OnGeometrySerialization)) + 0, collectionIndex, blasIndex));
	vec4 radius = imageLoad(globalGeometry, ivec3((indexOnCollection * (1 << expOfTwo_numOfVec4OnGeometrySerialization)) + 1, collectionIndex, blasIndex));

	return Geometry(center.xyz, radius.x);
}

#if !defined(GEOMETRY_READONLY)

/**
 * Update the geometry on the given BLAS, collection and relative position on the collection.
 *
 * Note: memory synchronization (memoryBarrierImage) must be performed by the caller!
 *
 * @param blasIndex the index of the BLAS
 * @param leafIndex the index of leaf on the given BLAS (corresponds to the index of geometry collection on the given BLAS)
 * @param indexOnCollection the index of the chosen collection that will be updated
 * @param geometry the geometry to be written
 */
void WriteGeometry_ByIndexes(const in uint blasIndex, const in uint leafIndex, const in uint indexOnCollection, const in Geometry geometry) {
	imageStore(globalGeometry, ivec3((indexOnCollection * (1 << expOfTwo_numOfVec4OnGeometrySerialization)) + 0, leafIndex, blasIndex), vec4(geometry.center, 1));
	imageStore(globalGeometry, ivec3((indexOnCollection * (1 << expOfTwo_numOfVec4OnGeometrySerialization)) + 1, leafIndex, blasIndex), vec4(geometry.radius, geometry.radius, geometry.radius, geometry.radius));
}
#endif

/**
 * Read the AABB on the given position.
 *
 * Note: memory synchronization (memoryBarrierImage) must be performed by the caller!
 *
 * @param blas the index of selected BLAS
 * @param index the position in the linearized tree
 * @return the AABB stored at the given index
 */
AABB ReadAABBFromBLAS_ByIndexes(const in uint blas, const in uint index) {
	return AABB(imageLoad(tlasBLAS, ivec2(2 * index, blas)), imageLoad(tlasBLAS, ivec2(2 * index + 1, blas)));
}

#if !defined(BLAS_READONLY)
/**
 * Update the AABB at the given index with the provided AABB.
 *
 * Note: memory synchronization (memoryBarrierImage) must be performed by the caller!
 *
 * @param blas the index of selected BLAS
 * @param index the position in the linearized tree
 * @param aabb the AABB to write
 */
void WriteAABBOnBLAS_ByIndexes(const in uint blas, const in uint index, const in AABB aabb) {
	imageStore(tlasBLAS, ivec2(2 * index, blas), aabb.position);
	imageStore(tlasBLAS, ivec2(2 * index + 1, blas), aabb.dimensions);
}
#endif

/**
 * Check if the given BLAS is empty.
 *
 * Note: an empty BLAS has the ModelMatrix set to an empty transform.
 *
 * @param index the index of the BLAS to be tested
 * @return TRUE iif the BLAS is empty
 */
bool isEmptyBLAS_ByIndex(const in uint index) {
	return ReadModelMatrix_ByIndex(index) == emptyTransform;
}

/**
 * Read the AABB on the given position.
 *
 * Note: memory synchronization (memoryBarrierImage) must be performed by the caller!
 *
 * @param index the position in the linearized tree
 * @return the AABB stored at the given index
 */
AABB ReadAABBFromTLAS_ByIndex(const in uint index) {
	return AABB(imageLoad(tlas, 2 * int(index)), imageLoad(tlas, 2 * int(index) + 1));
}

#if !defined(TLAS_READONLY)
/**
 * Update the AABB at the given index with the provided AABB.
 *
 * Note: memory synchronization (memoryBarrierImage) must be performed by the caller!
 *
 * @param index the position in the linearized tree
 * @param aabb the AABB to write
 */
void WriteAABBOnTLAS_ByIndex(const in uint index, const in AABB aabb) {
	imageStore(tlas, 2 * int(index), aabb.position);
	imageStore(tlas, 2 * int(index) + 1, aabb.dimensions);
}
#endif

AABB generateAABBFromGeometryOnBLASLeaf_ByBaseIndexOnGeometry(const in uint blasIndex, const in uint collectionIndex) {
	AABB bounding = emptyAABB;

	for (uint i = 0; i < (1 << expOfTwo_maxGeometryOnCollection); ++i) {
		bounding = expandAABBWithGeometry(bounding, ReadGeometry_ByIndexes(blasIndex, collectionIndex, i));
	}

	return bounding;
}

#if !defined(TLAS_READONLY)
void UpdateTLASFromLeafToRoot_ByIndex(const in uint indexOfLeafInTLAS) {
	uint indexOfLastUpdatedNodeInTLAS = NodeFromTLASLeaf_ByLeafNumber(indexOfLeafInTLAS);

	// On the leaf write the AABB aware of its associated Model Matrix
	WriteAABBOnTLAS_ByIndex(
		indexOfLastUpdatedNodeInTLAS,
		transformAABB(ReadAABBFromBLAS_ByIndexes(indexOfLeafInTLAS, 0), ReadModelMatrix_ByIndex(indexOfLeafInTLAS))
	);

	while (!isRootNode(indexOfLastUpdatedNodeInTLAS)) {
		memoryBarrierImage();

		// Target the parent node for update
		indexOfLastUpdatedNodeInTLAS = parentNode(indexOfLastUpdatedNodeInTLAS);

		// Update the current node with the two ModelMatrix-aware child nodes
		WriteAABBOnTLAS_ByIndex(
			indexOfLastUpdatedNodeInTLAS,
			joinAABBs(
				ReadAABBFromTLAS_ByIndex(leftNode(indexOfLastUpdatedNodeInTLAS)),
				ReadAABBFromTLAS_ByIndex(rightNode(indexOfLastUpdatedNodeInTLAS))
			)
		);
	}
}
#endif

#endif // CORE_GLSL