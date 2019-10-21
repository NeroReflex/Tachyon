#include "Ray.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Ray::Ray(const Ray& src) noexcept
	: mOrigin(src.mOrigin), mDirection(src.mDirection) {}

Ray::Ray(Ray&& src) noexcept
	: mOrigin(src.mOrigin), mDirection(src.mDirection) {}

Ray::Ray(glm::vec3 origin, glm::vec3 direction) noexcept
	: mOrigin(glm::vec4(std::move(origin), 1.0)),
	mDirection(glm::vec4(glm::normalize(direction), 0.0)) {}

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

glm::vec3 Ray::pointAt(const glm::float32& coeff) const noexcept {
	return getOrigin() + coeff * getDirection();
}
