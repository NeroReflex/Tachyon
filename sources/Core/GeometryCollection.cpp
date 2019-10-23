#include "GeometryCollection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

GeometryCollection::GeometryCollection() noexcept
	: mGeometryCount(0) {}

GeometryCollection::GeometryCollection(const GeometryCollection& src) noexcept
	: mGeometryCount(src.mGeometryCount),
	mGeometry(src.mGeometry) {}

GeometryCollection::GeometryCollection(const std::initializer_list<Geometry>& geometryCollection) noexcept
	: GeometryCollection() {
	// Add each geometric shape to the list starting from the first position
	for (const auto& geometry : geometryCollection)
		push(geometry);
}

void  GeometryCollection::push(const Geometry& geometry) noexcept {
	DBG_ASSERT(mGeometryCount < GeometryCollection::maxNumber);

	// Store the geometry and increase the counter of stored geometry
	mGeometry[mGeometryCount++] = geometry;
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

		if ((currentHit) && (closestHit.getDistance() > isecInfo.getDistance())) {
			closestHit = isecInfo;
		}

		hit = (currentHit) ? true : hit;
	});

	// Return the hit closer to the camera (checking if a previous one exists already)
	if (closestHit.getDistance() < isecInfo.getDistance()) {
		isecInfo = closestHit;
	}

	return hit;
}

size_t GeometryCollection::getMaxLinearBufferSize() noexcept {
	return Geometry::getMaxLinearBufferSize() * GeometryCollection::maxNumber;
}

size_t GeometryCollection::getLinearBufferSize() const noexcept {
	return GeometryCollection::getMaxLinearBufferSize();
}

void GeometryCollection::linearizeToBuffer(void* buffer) const noexcept {
	// TODO: linearize along with used location count...
	for (size_t i = 0; i < GeometryCollection::maxNumber; ++i) {
		void* bufferLocation = reinterpret_cast<void*>(reinterpret_cast<char*>(buffer) + (Geometry::getMaxLinearBufferSize() * i));
		if (i < mGeometryCount) {
			const auto& currentGeometry = mGeometry[i];
			currentGeometry.linearizeToBuffer(bufferLocation);
		} else {
			Geometry::linearizeEmptyToBuffer(bufferLocation);
		}
	}
}
