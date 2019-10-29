#include "Camera.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Camera::Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, glm::float32 vfov, glm::float32 aspect) noexcept 
	: mFoV(std::move(vfov)),
	mLookingDir(std::move(lookat)),
	mUPVector(std::move(vup)),
	mOrigin(std::move(lookfrom)) {
	setAspect(aspect);
}

void Camera::update() noexcept {
	glm::vec3 u, v, w;
	const glm::float32 theta = mFoV * glm::pi<glm::float32>() / 180;
	const glm::float32 half_height = glm::tan(theta / 2.0);
	const glm::float32 half_width = mAspectRatio * half_height;
	w = glm::normalize(mOrigin - mLookingDir);
	u = glm::normalize(glm::cross(mUPVector, w));
	v = glm::cross(w, u);
	mLowerLeftCorner = mOrigin - half_width * u - half_height * v - w;
	mHorizontal = static_cast<glm::float32>(2.0) * (u * half_width);
	mVertical = static_cast<glm::float32>(2.0) * (v * half_height);
}

void Camera::setAspect(glm::float32 aspect) noexcept {
	mAspectRatio = aspect;
	update();
}

glm::vec3 Camera::getUp() const noexcept {
	return mUPVector;
}

glm::vec3 Camera::getLowerLeftCorner() const noexcept {
	return mLowerLeftCorner;
}

glm::vec3 Camera::getHorizontal() const noexcept {
	return mHorizontal;
}

glm::vec3 Camera::getVertical() const noexcept {
	return mVertical;
}

Ray Camera::generate(glm::float32 s, glm::float32 t) const noexcept {
	return Ray(mOrigin, mLowerLeftCorner + s * mHorizontal + t * mVertical - mOrigin);
}

glm::vec3 Camera::getCameraPosition() const noexcept {
	return mOrigin;
}
