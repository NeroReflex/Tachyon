#include "GeometryCollection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

GeometryCollection::GeometryCollection() noexcept
	: mGeometryCount(0) {}

GeometryCollection::GeometryCollection(const GeometryCollection& src) noexcept
	: mGeometryCount(src.mGeometryCount),
	mGeometry(src.mGeometry) {}

GeometryCollection::GeometryCollection(const std::initializer_list<Geometry>& geometryCollection) noexcept
	: mGeometryCount(geometryCollection.size()) {
	size_t i = 0;
	for (const auto& geometry : geometryCollection) {
		mGeometry[i++] = geometry;
	}
}

glm::uint32 GeometryCollection::getSize() const noexcept {
	return mGeometryCount;
}

bool GeometryCollection::hit(const Ray& ray) const noexcept {
	for (size_t i = 0; i < mGeometryCount; ++i) {
		if (mGeometry[i].hit(ray)) {
			return true;
		}
	}

	return false;
}