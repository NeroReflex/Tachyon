#include "GeometryPrimitive.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

GeometryPrimitive::GeometryPrimitive(glm::vec3 position, glm::float32 radius) noexcept
	: glslData(glm::vec4(position, radius)) {}