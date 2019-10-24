#include "GeometryCollection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

bool GeometryCollection::isHitBy(const Ray& ray) const noexcept {
	bool hit = false;

	Collection<Geometry, 3>::foreach([&hit, ray](const Geometry& geometry) {
		hit = (geometry.isHitBy(ray)) ? true : hit;
	});

	return hit;
}

bool GeometryCollection::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept {
	bool hit = false;
	RayGeometryIntersection closestHit;

	Collection<Geometry, 3>::foreach([&hit, ray, &closestHit, &isecInfo, minDistance, maxDistance](const Geometry& geometry) {
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
