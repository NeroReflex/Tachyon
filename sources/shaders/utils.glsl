#ifndef UTILS_GLSL
#define UTILS_GLSL

const float PI = 3.14159265359;
const float infinity = (1.0) / 0.0;
const float minusInfinity = (-1.0) / 0.0;
const mat4 identityTransform = mat4(1);
const mat4 emptyTransform = mat4(0);

 /**
  * Swaps two numbers.
  *
  * @param a the first number
  * @param b the second number
  */
void swap(inout float a, inout float b) {
	float temp = a;
	a = b;
	b = temp;
}

/**
 * Finds the minimum component on a vector.
 *
 * @param a the vector
 * @return the minimum component
 */
float min_component(vec3 a) {
	return min(min(a.x, a.y), a.z);
}

/**
 * Finds the maximum component on a vector.
 *
 * @param a the vector
 * @return the maximum component
 */
float max_component(vec3 a) {
	return max(max(a.x, a.y), a.z);
}

/**
 * Checks if the given number is even.
 *
 * @param n the number to check
 * @return TRUE iif the given number is even
 */
bool isEven(const in uint n) {
	return (n & 0x0001) == 0;
}

/**
 * Given a node of a binary tree with a fixed structure with index i,
 * returns true if the node is the root of the entire structure.
 *
 * @param i the node index on the tree-array
 * @return TRUE iif the index is the root of the tree-array
 */
bool isRootNode(const in uint i) {
	return i == 0;
}

/**
 * Given a node of a binary tree with a fixed structure with index i,
 * returns the index of its parent node in the same fixed structure.
 *
 * @param i the node index on the tree-array
 * @return the parent node index on the tree-array
 */
uint parentNode(const in uint i) {
	return (i - 1) / 2;
}

/**
 * Given a node of a binary tree with a fixed structure with index i,
 * returns the index of its left childer node in the same fixed structure.
 *
 * @param i the node index on the tree-array
 * @return the left childern node index on the tree-array
 */
uint leftNode(const in uint i) {
	return (2 * i + 1);
}

/**
 * Given a node of a binary tree with a fixed structure with index i,
 * returns the index of its right childer node in the same fixed structure.
 *
 * @param i the node index on the tree-array
 * @return the right childern node index on the tree-array
 */
uint rightNode(const in uint i) {
	return (2 * i + 2);
}

#endif // UTILS_GLSL