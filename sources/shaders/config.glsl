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

// Ora sto usando le sfere, quindi mi basta 1 vec4 per serializzxarne una. Dovessi usare i triangoli mi serviranno più vettori.
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

#define UPDATE_WORKGROUP_X 256

#define FLUSH_WORKGROUP_X 256

#define INSERT_WORKGROUP_X 16
#define INSERT_WORKGROUP_Y 16
#define INSERT_WORKGROUP_Z 1

#define RENDER_WORKGROUP_X 16
#define RENDER_WORKGROUP_Y 16
#define RENDER_WORKGROUP_Z 1

#define CORE_BINDING 0

// TEXTUREs binding
#define OUTPUT_BINDING CORE_BINDING+5

// UNIFORMs binding
#define UNIFORM_BASE CORE_BINDING+7
#define CAMERA_BINDING UNIFORM_BASE+0
#define HDR_BINDING UNIFORM_BASE+8
#define GEOMETRY_INSERTT_ATTR_BINDING UNIFORM_BASE+9

// BUFFERs binding
#define BUFFER_BASE CORE_BINDING
#define TLAS_BINDING BUFFER_BASE+0
#define BLAS_BINDING BUFFER_BASE+1
#define BLAS_ATTRIBUTES_BINDING BUFFER_BASE+2
#define GEOMETRY_BINDING BUFFER_BASE+3
#define GEOMETRY_INSERT_BINDING BUFFER_BASE+4

#endif // CONFIG_GLSL