#include "Light.h"

using namespace Tachyon;
using namespace Tachyon::Core;
using namespace Tachyon::Core::Lighting;

Light::Light(const glm::vec3& color, const glm::float32& intensity) noexcept
	: mColor(glm::vec4(color, intensity)) {}

glm::vec3 Light::getColor() const noexcept {
	return mColor;
}

glm::float32 Light::getIntensity() const noexcept {
	return mColor.a;
}
