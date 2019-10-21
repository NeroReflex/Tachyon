#include "RayGeometryIntersection.h"

using namespace Tachyon;
using namespace Tachyon::Core;

RayGeometryIntersection::RayGeometryIntersection() noexcept
	: mSurfaceContactPoint(glm::vec4(std::numeric_limits<glm::float32>::quiet_NaN())),
	mSurfaceDistance(std::numeric_limits<glm::float32>::infinity()),
	mSurfaceNormal(glm::vec4(std::numeric_limits<glm::float32>::quiet_NaN())) {}

RayGeometryIntersection::RayGeometryIntersection(glm::vec3 contactPoint, glm::float32 distance, glm::vec3 normal) noexcept
	: mSurfaceContactPoint(std::move(contactPoint)),
	mSurfaceDistance(std::move(distance)),
	mSurfaceNormal(glm::vec4(std::move(glm::normalize(normal)), 0.0)) {}

RayGeometryIntersection::RayGeometryIntersection(const RayGeometryIntersection& src) noexcept
	: mSurfaceContactPoint(src.mSurfaceContactPoint),
	mSurfaceDistance(src.mSurfaceDistance),
	mSurfaceNormal(src.mSurfaceNormal) {}

RayGeometryIntersection::RayGeometryIntersection(RayGeometryIntersection&& src) noexcept
	: mSurfaceContactPoint(src.mSurfaceContactPoint),
	mSurfaceDistance(src.mSurfaceDistance),
	mSurfaceNormal(src.mSurfaceNormal) {}

RayGeometryIntersection& RayGeometryIntersection::operator=(const RayGeometryIntersection& src) noexcept {
	if (&src != this) {
		mSurfaceContactPoint = src.mSurfaceContactPoint;
		mSurfaceDistance = src.mSurfaceDistance;
		mSurfaceNormal = src.mSurfaceNormal;
	}

	return *this;
}

glm::float32 RayGeometryIntersection::getDistance() const noexcept {
	return mSurfaceDistance;
}

glm::vec3 RayGeometryIntersection::getNormal() const noexcept {
	return mSurfaceNormal;
}
