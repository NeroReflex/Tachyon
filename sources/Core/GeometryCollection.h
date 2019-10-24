#pragma once

#include "Collection.hpp"
#include "Geometry.h"

namespace Tachyon {
	namespace Core {
		class GeometryCollection :
			public Collection<Geometry, 3>,
			virtual public RayInterceptable {
		public:

			using Collection<Geometry, 3>::Collection;

			bool isHitBy(const Ray& ray) const noexcept override;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;
		};
	}
}