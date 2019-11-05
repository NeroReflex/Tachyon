#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {

		class GeometryPrimitive {
		public:
			/**
			 * Construct the geometric primitive.
			 * The default is an invalid geometry (one that cannot be intersected and its has have no volume)
			 */
			GeometryPrimitive(glm::vec3 position = glm::vec3(0, 0, 0), glm::float32 radius = 0.0) noexcept;

			~GeometryPrimitive() = default;
			
		private:
			glm::vec4 glslData;
		};

	}
}