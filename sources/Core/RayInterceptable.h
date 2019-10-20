#pragma once

#include "RayGeometryIntersection.h"

namespace Tachyon {
	namespace Core {
		class RayInterceptable {
		public:
			virtual bool isHitBy(const Ray& ray) const noexcept = 0;

			virtual bool intersection(const Ray& ray, RayGeometryIntersection& isecInfo) const noexcept = 0;
		};
	}
}