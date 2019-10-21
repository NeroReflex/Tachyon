#pragma once

#include "Core/RayGeometryIntersection.h"

namespace Tachyon {
	namespace Rendering {
		namespace Shading {
			class Shader {
			public:
				virtual glm::vec3 operator()(const Core::RayGeometryIntersection& isect) const noexcept = 0;
			};
		}
	}
}