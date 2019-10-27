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
#include <GL/gl3w.h>

// GLFW
#include <GLFW/glfw3.h>

typedef glm::uint32 UnsignedType;
typedef glm::float32 NumericType;

#define DBG_ASSERT(x) assert(x)
#define DBG_ONLY(x) x

// Remove macros that generate compile errors
#undef max
#undef min