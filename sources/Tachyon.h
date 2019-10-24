#pragma once

// STL streams
#include <iostream>
#include <fstream>

// STL data types
#include <functional>
#include <string>

// STL containers
#include <array>
#include <initializer_list>
#include <vector>

// STL memory
#include <memory>

// STL algorithms
#include <algorithm>
#include <utility>

// GLM math library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

typedef glm::float32 NumericType;

#define DBG_ASSERT(x) assert(x)