#pragma once

#include "Geometry.h"

namespace Tachyon {
	namespace Core {
		class GeometryCollection :
			virtual public Linearizable,
			virtual public RayInterceptable {

			constexpr static glm::uint32 maxNumber = 16;

		public:
			GeometryCollection() noexcept;

			GeometryCollection(const std::initializer_list<Geometry>& geometryCollection) noexcept;

			GeometryCollection(const GeometryCollection& src) noexcept;

			glm::uint32 getSize() const noexcept;

			bool isHitBy(const Ray& ray) const noexcept override;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;

			static size_t getMaxLinearBufferSize() noexcept;

			size_t getLinearBufferSize() const noexcept final;

			void linearizeToBuffer(void* buffer) const noexcept final;

			void push(const Geometry& geometry) noexcept;

		private:
			std::array<Geometry, maxNumber> mGeometry;

			glm::uint32 mGeometryCount;
		};
	}
}