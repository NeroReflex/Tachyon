#ifndef CORE_GLSL
#define CORE_GLSL

#include "config.glsl"
#include "utils.glsl"
#include "geometry.glsl"
#include "aabb.glsl"
#include "bvh.glsl"

layout(std140, binding = TLAS_BINDING) TLAS_MEMORY_MODEL buffer TLAS_Buffer {
	AABB tlas[];
};

layout(std140, binding = BLAS_BINDING) BLAS_MOEMORY_MODEL buffer BLAS_Buffer {
	AABB tlasBLAS[];
};

layout(std140, binding = GEOMETRY_BINDING) GEOMETRY_MEMORY_MODEL buffer Triangles_Buffer {
	Triangle globalGeometry[];
};

layout(std140, binding = BLAS_ATTRIBUTES_BINDING) GEOMETRY_MEMORY_MODEL buffer ModelMatrices_Buffer {
	mat4 ModelMatrix[];
};

mat4 ReadModelMatrix_ByIndex(const in uint index) {
	return ModelMatrix[index];
}

#if !defined(MODELMATRIX_READONLY)
void WriteModelMatrix_ByIndex(const in uint index, const in mat4 mMatrix) {
	ModelMatrix[index] = mMatrix;
}
#endif

Geometry ReadGeometry_ByIndexes(const in uint blasIndex, const in uint collectionIndex, const in uint indexOnCollection) {
	uint geometryIndex = blasIndex * ((1 << expOfTwo_maxCollectionsForModel) * (1 << expOfTwo_maxGeometryOnCollection)) +
		collectionIndex * (1 << expOfTwo_maxGeometryOnCollection) +
		indexOnCollection;

	return Geometry(globalGeometry[geometryIndex].vertices[0].xyz, globalGeometry[geometryIndex].vertices[1].x);
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
void WriteGeometry_ByIndexes(const in uint blasIndex, const in uint collectionIndex, const in uint indexOnCollection, const in Geometry geometry) {
	uint geometryIndex = blasIndex * ((1 << expOfTwo_maxCollectionsForModel) * (1 << expOfTwo_maxGeometryOnCollection)) +
		collectionIndex * (1 << expOfTwo_maxGeometryOnCollection) +
		indexOnCollection;

	globalGeometry[geometryIndex].vertices[0] = vec4(geometry.center.xyz, geometry.radius);
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
	uint aabbIndex = blas * (1 << numberOfTreeElementsToContainExpOfTwoLeafs(expOfTwo_maxCollectionsForModel)) + index;

	return tlasBLAS[aabbIndex];
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
	uint aabbIndex = blas * (1 << numberOfTreeElementsToContainExpOfTwoLeafs(expOfTwo_maxCollectionsForModel)) + index;

	tlasBLAS[aabbIndex] = aabb;
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
	return tlas[index];
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
	tlas[index] = aabb;
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