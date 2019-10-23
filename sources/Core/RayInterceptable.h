#pragma once

#include "RayGeometryIntersection.h"

namespace Tachyon {
	namespace Core {
		class RayInterceptable {
		public:
			//virtual ~RayInterceptable() = default;

			virtual bool isHitBy(const Ray& ray) const noexcept = 0;

			virtual bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept = 0;
		};
	}
}