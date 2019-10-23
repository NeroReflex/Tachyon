#include "PointLight.h"

using namespace Tachyon;
using namespace Tachyon::Core;
using namespace Tachyon::Core::Lighting;

PointLight::PointLight(const glm::vec3& color, const glm::float32& intensity, const glm::vec3& position) noexcept
    : Light(color, intensity), mPosition(glm::vec4(position, 1.0)) {}

glm::vec3 PointLight::getPosition() const noexcept {
	return mPosition;
}