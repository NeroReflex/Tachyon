#include "GeometryCollection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

bool GeometryCollection::isHitBy(const Ray& ray, glm::mat4 transform) const noexcept {
	bool hit = false;

	Collection<Geometry, 3>::foreach([&hit, ray, transform](const Geometry& geometry) {
		hit = (geometry.isHitBy(ray, transform)) ? true : hit;
	});

	return hit;
}

bool GeometryCollection::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform) const noexcept {
	bool hit = false;
	RayGeometryIntersection closestHit;

	Collection<Geometry, 3>::foreach([&hit, ray, &closestHit, &isecInfo, minDistance, maxDistance, transform](const Geometry& geometry) {
		bool currentHit = geometry.intersection(ray, minDistance, maxDistance, isecInfo, transform);

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

AABB GeometryCollection::bvBase() const noexcept {
	std::vector<glm::vec3> vertices;

	foreach([this, &vertices](const Geometry& geometry) {
		// Build an AABB from the current geometry
		const auto currentAABB = geometry.bvBase();

		const auto currentAABBVertices = currentAABB.getVertices();

		vertices.insert(vertices.end(), currentAABBVertices.cbegin(), currentAABBVertices.cend());
	});

	return AABB(vertices);
}