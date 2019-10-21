#include "Camera.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Camera::Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, glm::float32 vfov, glm::float32 aspect) noexcept 
	: mOrigin(lookfrom) {
	// vfov is top to bottom in degrees
	glm::vec3 u, v, w;
	const glm::float32 theta = vfov * glm::pi<glm::float32>() / 180;
	const glm::float32 half_height = glm::tan(theta / 2.0);
	const glm::float32 half_width = aspect * half_height;
	w = glm::normalize(lookfrom - lookat);
	u = glm::normalize(glm::cross(vup, w));
	v = glm::cross(w, u);
	mLowerLeftCorner = mOrigin - half_width * u - half_height * v - w;
	mHorizontal = static_cast<glm::float32>(2.0) * (u * half_width);
	mVertical = static_cast<glm::float32>(2.0) * (v * half_height);
}

Ray Camera::generate(glm::float32 s, glm::float32 t) const noexcept {
	return Ray(mOrigin, mLowerLeftCorner + s * mHorizontal + t * mVertical - mOrigin);
}
