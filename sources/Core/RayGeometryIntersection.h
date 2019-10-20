#pragma once

#include "Ray.h"

namespace Tachyon {
	namespace Core {
		class RayGeometryIntersection {
		public:
			RayGeometryIntersection() noexcept;

			RayGeometryIntersection(glm::vec3 contactPoint, glm::float32 distance, glm::vec3 normal) noexcept;

			glm::float32 getDistance() const noexcept;

			glm::vec3 getNormal() const noexcept;

		private:
			glm::vec3 mSurfaceContactPoint;
			glm::float32 mSurfaceDistance;
			glm::vec4 mSurfaceNormal;
		};
	}
}