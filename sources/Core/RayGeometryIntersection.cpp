#include "RayGeometryIntersection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

RayGeometryIntersection::RayGeometryIntersection() noexcept
	: mSurfaceContactPoint(std::numeric_limits<glm::float32>::infinity()) {}

RayGeometryIntersection::RayGeometryIntersection(glm::vec3 contactPoint, glm::float32 distance, glm::vec3 normal) noexcept
	: mSurfaceContactPoint(std::move(contactPoint)),
	mSurfaceDistance(std::move(distance)),
	mSurfaceNormal(glm::vec4(std::move(glm::normalize(normal)), 0.0)) {}

glm::float32 RayGeometryIntersection::getDistance() const noexcept {
	return mSurfaceDistance;
}

glm::vec3 RayGeometryIntersection::getNormal() const noexcept {
	return mSurfaceNormal;
}
