#pragma once

#include "Collection.hpp"
#include "Geometry.h"

namespace Tachyon {
	namespace Core {
		class GeometryCollection :
			public Collection<Geometry, expOfTwoOfMaxGeometryElementsInCollection>,
			virtual public RayInterceptable {

		public:

			using Collection<Geometry, expOfTwoOfMaxGeometryElementsInCollection>::Collection;

			AABB bvBase() const noexcept;

			/**
			 * Check if any of the stored elements is hit by the given ray.
			 *
			 * @return true on match
			 */
			bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept final;

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
			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept final;

			static constexpr size_t getBufferSize() noexcept {
				// The required memory space in bytes is the number of bytes required by the serialization of the template type,
				// multiplied by the maximum number of serializable elements, plus an uint32 used to store the number of effectively serialized elements.
				return(Geometry::getBufferSize() * Collection<Geometry, expOfTwoOfMaxGeometryElementsInCollection>::maxNumber) + sizeof(glm::uint);
			};

			static void linearizeToBuffer(const GeometryCollection& src, void* dst) noexcept {
				const glm::uint occupiedElementsCount = src.getSize();

				// Linearize elements of collection (according to std140 rules)
				for (size_t i = 0; i < occupiedElementsCount; ++i) {
					void* bufferLocation = reinterpret_cast<void*>(reinterpret_cast<char*>(dst) + (Geometry::getBufferSize() * i));

					Geometry::linearizeToBuffer(src[i], bufferLocation);
				}

				// Append the number of linearized elements at the beginning...
				glm::uint32* bufferSerializedCount = reinterpret_cast<glm::uint32*>(reinterpret_cast<char*>(dst) + (Geometry::getBufferSize() * Collection<Geometry, expOfTwoOfMaxGeometryElementsInCollection>::maxNumber));
				*bufferSerializedCount = occupiedElementsCount;
			}
		};
	}
}