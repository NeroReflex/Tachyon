#pragma once

#include "Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace Shading {
			class DistanceShader :
				virtual public Shader {
			public:
				glm::vec3 operator()(const Core::RayGeometryIntersection& isect) const noexcept final;
			};
		}
	}
}