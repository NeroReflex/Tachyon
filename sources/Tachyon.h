#pragma once

// STL streams
#include <iostream>
#include <fstream>
#include <sstream>

// STL data types
#include <functional>
#include <string>

// STL containers
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include <array>
#include <list>
#include <set>

// STL memory
#include <memory>

// STL algorithms
#include <algorithm>
#include <utility>
#include <limits>

// GLM math library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

// OpenGL
#if defined(OPENGL_SUPPORT)
#include <GL/gl3w.h>
#endif

#if defined(VULKAN_SUPPORT)
#include <vulkan/vulkan.h>

#if !defined(NDEBUG)
#define VULKAN_ENABLE_VALIDATION_LAYERS 1
#define VK_CHECK_RESULT(f) 																				\
{																										\
    VkResult res = (f);																					\
    if (res != VK_SUCCESS)																				\
    {																									\
        printf("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__); \
        assert(res == VK_SUCCESS);																		\
    }																									\
}
#else
VK_CHECK_RESULT(f) f
#endif

#endif

// GLFW
#include <GLFW/glfw3.h>

typedef glm::uint32 UnsignedType;
typedef glm::float32 NumericType;

#if !defined(NDEBUG)
	#define DBG_ASSERT(x) assert(x)
	#define DBG_ONLY(x) x
#else
	#define DBG_ASSERT(x) /* x */
	#define DBG_ONLY(x) /* x */
#endif

// Remove macros that generate compile errors
#undef max
#undef min

constexpr size_t roundupToMultipleOfVec4(size_t in) {
	return (in / sizeof(glm::vec4)) + sizeof(glm::vec4);
}
