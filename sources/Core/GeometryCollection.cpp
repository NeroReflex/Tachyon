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
	glm::uint32 i = 0;

	// Add each geometric shape to the list starting from the first position
	for (const auto& geometry : geometryCollection)
		mGeometry[i++] = geometry;
}

glm::uint32 GeometryCollection::getSize() const noexcept {
	return mGeometryCount;
}

bool GeometryCollection::isHitBy(const Ray& ray) const noexcept {
	bool hit = false;

	std::for_each(mGeometry.cbegin(), mGeometry.cbegin() + mGeometryCount, [&hit, ray](const Geometry& geometry) {
		hit = (geometry.isHitBy(ray)) ? true : hit;
	});

	return hit;
}

bool GeometryCollection::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept {
	bool hit = false;
	RayGeometryIntersection closestHit;

	std::for_each(mGeometry.cbegin(), mGeometry.cbegin() + mGeometryCount, [&hit, ray, &closestHit, &isecInfo, minDistance, maxDistance](const Geometry& geometry) {
		bool currentHit = geometry.intersection(ray, minDistance, maxDistance, isecInfo);

		if ((currentHit && !hit) || ((hit && currentHit) && (closestHit.getDistance() > isecInfo.getDistance()))) {
			closestHit = isecInfo;
		}

		hit = (currentHit) ? true : hit;
	});

	return hit;
}
