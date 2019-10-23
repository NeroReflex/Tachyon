#pragma once

#include "Geometry.h"

namespace Tachyon {
	namespace Core {
		class GeometryCollection :
			virtual public RayInterceptable {

			constexpr static glm::uint32 maxNumber = 16;

		public:
			GeometryCollection() noexcept;

			GeometryCollection(const std::initializer_list<Geometry>& geometryCollection) noexcept;

			GeometryCollection(const GeometryCollection& src) noexcept;

			glm::uint32 getSize() const noexcept;

			bool isHitBy(const Ray& ray) const noexcept override;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;

			void push(const Geometry& geometry) noexcept;

			static constexpr size_t getBufferSize() noexcept {
				return Geometry::getBufferSize() * GeometryCollection::maxNumber;
			};

			static void linearizeToBuffer(const GeometryCollection& src, void* dst) noexcept {
				// TODO: linearize along with used location count...
				for (size_t i = 0; i < GeometryCollection::maxNumber; ++i) {
					void* bufferLocation = reinterpret_cast<void*>(reinterpret_cast<char*>(dst) + (Geometry::getBufferSize() * i));
					if (i < src.mGeometryCount) {
						const auto& currentGeometry = src.mGeometry[i];
						Geometry::linearizeToBuffer(currentGeometry, bufferLocation);
					} else {
						Geometry::linearizeEmptyToBuffer(bufferLocation);
					}
				}
			}

		private:
			std::array<Geometry, maxNumber> mGeometry;

			glm::uint32 mGeometryCount;
		};
	}
}