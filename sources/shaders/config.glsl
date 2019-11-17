#ifndef CONFIG_GLSL
#define CONFIG_GLSL

/*
#define expOfTwo_maxModels 9
#define expOfTwo_maxGeometryOnCollection 3
// TODO: investigate why setting this to 3 or less makes the CS crash
#define expOfTwo_maxCollectionsForModel 12
*/
#define expOfTwo_maxModels 2
#define expOfTwo_maxGeometryOnCollection 3
// TODO: investigate why setting this to 3 or less makes the CS crash
#define expOfTwo_maxCollectionsForModel 4

#define expOfTwo_numOfVec4OnGeometrySerialization 1

#define numberOfTreeElementsToContainExpOfTwoLeafs( expOfTwo ) ((1 << (expOfTwo+1))-1)

#if defined(RENDER)
	#define TLAS_MEMORY_MODEL readonly
	#define TLAS_READONLY

	#define BLAS_MOEMORY_MODEL readonly
	#define BLAS_READONLY

	#define GEOMETRY_MEMORY_MODEL readonly
	#define GEOMETRY_READONLY

	#define MODELMATRIX_MEMORY_MODEL readonly
	#define MODELMATRIX_READONLY

	#elif defined(TLAS_UPDATE)
	#define TLAS_MEMORY_MODEL coherent

	#define BLAS_MOEMORY_MODEL readonly
	#define BLAS_READONLY

	#define GEOMETRY_MEMORY_MODEL readonly
	#define GEOMETRY_READONLY

	#define MODELMATRIX_MEMORY_MODEL readonly
	#define MODELMATRIX_READONLY
#else
	#define TLAS_MEMORY_MODEL coherent
	#define BLAS_MOEMORY_MODEL coherent
	#define GEOMETRY_MEMORY_MODEL coherent
	#define MODELMATRIX_MEMORY_MODEL coherent
#endif

// UNIFORMs bindings
#define CAMERA_BINDING 6
#define HDR_BINDING 7

// TEXTUREs binding
#define TLAS_BINDING 0
#define BLAS_BINDING 1
#define GEOMETRY_BINDING 2
#define BLAS_ATTRIBUTES_BINDING 3
#define OUTPUT_BINDING 4
#define DEBUG_BINDING 5

#endif // CONFIG_GLSL