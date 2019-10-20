#include "Ray.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Ray::Ray(const Ray& src) noexcept
	: mOrigin(src.mOrigin), mDirection(src.mDirection) {}

Ray::Ray(glm::vec3 origin, glm::vec3 direction) noexcept
	: mOrigin(glm::vec4(std::move(origin), 1)),
	mDirection(glm::vec4(glm::normalize(std::move(direction)), 1)) {}

Ray& Ray::operator=(const Ray& src) noexcept {
	mOrigin = src.mOrigin;
	mDirection = src.mDirection;

	return *this;
}

glm::vec3 Ray::getOrigin() const noexcept {
	return mOrigin;
}

glm::vec3 Ray::getDirection() const noexcept {
	return mDirection;
}