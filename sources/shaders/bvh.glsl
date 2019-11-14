#ifndef BVH_GLSL
#define BVH_GLSL

/**
 * Check if the BLAS node at the given index is a leaf.
 *
 * @param index the node index on the linearized BVH-tree
 * @return TRUE iif the node is a leaf
 */
bool isBLASNodeLeaf_ByIndex(const in uint leafIndex) {
	return leafIndex >= ((1 << expOfTwo_maxCollectionsForModel) - 1);
}

/**
 * Check if the TLAS node at the given index is a leaf.
 *
 * @param index the node index on the linearized BVH-tree
 * @return TRUE iif the node is a leaf
 */
bool isTLASNodeLeaf_ByIndex(const in uint leafIndex) {
	return leafIndex >= ((1 << expOfTwo_maxModels) - 1);
}

/**
 * Get the index of a leaf element from the index of that leaf node.
 *
 * @param leafIndex the index used to access the leaf on the BLAS
 * @return the index of the geometry collection
 */
uint LeafFromBLASNode_ByIndex(const in uint leafIndex) {
	return leafIndex - ((1 << expOfTwo_maxCollectionsForModel) - 1);
}

/**
 * Get the index of a leaf element from the index of that leaf node.
 *
 * @param leafIndex the index used to access the leaf on the TLAS
 * @return the index of the BLAS
 */
uint LeafFromTLASNode_ByIndex(const in uint leafIndex) {
	return leafIndex - ((1 << expOfTwo_maxModels) - 1);
}

/**
 * Get the index of a leaf element from the index of that leaf node.
 *
 * @param leafIndex the index used to access the leaf on the TLAS
 * @return the index of the BLAS
 */
uint NodeFromBLASLeaf_ByLeafNumber(const in uint leafIndex) {
	return leafIndex + ((1 << expOfTwo_maxCollectionsForModel) - 1);
}

/**
 * Get the index of a leaf element from the index of that leaf node.
 *
 * @param leafIndex the index used to access the leaf on the TLAS
 * @return the index of the BLAS
 */
uint NodeFromTLASLeaf_ByLeafNumber(const in uint leafIndex) {
	return leafIndex + ((1 << expOfTwo_maxModels) - 1);
}

#endif // BVH_GLSL
