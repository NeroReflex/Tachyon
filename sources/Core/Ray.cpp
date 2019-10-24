#include "Ray.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Ray::Ray(const Ray& src) noexcept
	: mOrigin(src.mOrigin),
	mDirection(src.mDirection),
	mInvDirection(glm::vec4(1 / mDirection.x, 1 / mDirection.y, 1 / mDirection.z, 0)),
	mSign(src.mSign) {}

Ray::Ray(Ray&& src) noexcept
	: mOrigin(std::move(src.mOrigin)),
	mDirection(std::move(src.mDirection)),
	mInvDirection(std::move(src.mInvDirection)),
	mSign(std::move(src.mSign)) {}

Ray::Ray(glm::vec3 origin, glm::vec3 direction) noexcept
	: mOrigin(glm::vec4(std::move(origin), 1.0)),
	mDirection(glm::vec4(glm::normalize(direction), 0.0)) ,
	mInvDirection(glm::vec4(1 / mDirection.x, 1 / mDirection.y, 1 / mDirection.z, 0)) {
		mSign[0] = (mInvDirection.x < 0)/* ? 1 : 0*/;
		mSign[1] = (mInvDirection.y < 0)/* ? 1 : 0*/;
		mSign[2] = (mInvDirection.z < 0)/* ? 1 : 0*/;
}

Ray& Ray::operator=(const Ray& src) noexcept {
	mOrigin = src.mOrigin;
	mDirection = src.mDirection;
	mInvDirection = src.mInvDirection;
	mSign = src.mSign;

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


const glm::vec4& Ray::getInvDirection() const noexcept {
	return mInvDirection;
}

const std::array<glm::int16, 3>& Ray::getSigns() const noexcept {
	return mSign;
}
