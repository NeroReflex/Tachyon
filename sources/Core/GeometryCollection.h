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

			AABB bvBase() const noexcept;

			/**
			 * Check if any of the stored elements is hit by the given ray.
			 *
			 * @return true on match
			 */
			bool isHitBy(const Ray& ray) const noexcept override;

			/**
			 * Check if any of the stored elements is hit by the given ray and the hitpoint distance is between minDistance and MaxDistance,
			 * in case of positive result the RayGeometryIntersection object is populated with the nearest hit that respects the criteria.
			 *
			 * @param ray the ray to be tested
			 * @param minDistance the minimum allowed distance
			 * @param maxDistance the maximum allowed distance
			 * @param isecInfo the ray-geometry intersection filled with collision data
			 *
			 * @return true on match
			 */
			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;
		};
	}
}