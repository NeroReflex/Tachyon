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

			bool hit(const Ray& ray) const noexcept override;

		private:
			std::array<Geometry, maxNumber> mGeometry;

			glm::uint32 mGeometryCount;
		};
	}
}