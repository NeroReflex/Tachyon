#include "Camera.h"

using namespace Tachyon;
using namespace Tachyon::Core;

const glm::vec3 Camera::UPVector = glm::vec3(0, 1, 0);

Camera::Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::float32 vfov, glm::float32 aspect, glm::vec3 vup) noexcept
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

void Tachyon::Core::Camera::setAspect(glm::uint32 width, glm::uint32 height) noexcept
{
	setAspect(static_cast<glm::float32>(width) / static_cast<glm::float32>(height));
}

glm::vec3 Camera::getUp() const noexcept {
	return mUPVector;
}

glm::vec3 Camera::getRaytracingLowerLeftCorner() const noexcept {
	return mLowerLeftCorner;
}

glm::vec3 Camera::getRaytracingHorizontal() const noexcept {
	return mHorizontal;
}

glm::vec3 Camera::getRaytracingVertical() const noexcept {
	return mVertical;
}

glm::vec3 Camera::getCameraPosition() const noexcept {
	return mOrigin;
}

glm::vec3 Camera::getViewDirection() const noexcept {
	return mLookingDir;
}

glm::float32 Camera::getFieldOfView() const noexcept {
	return mFoV;
}

glm::float32 Camera::getAspectRatio() const noexcept
{
	return mAspectRatio;
}
