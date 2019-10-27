#include "ExposureToneMapper.h"

using namespace Tachyon;
using namespace Tachyon::Rendering::CPU;

ExposureToneMapper::ExposureToneMapper(glm::float32 gamma, glm::float32 exposure) noexcept
	: mGamma(std::move(gamma)), mExposure(std::move(exposure)) {}

glm::vec3 ExposureToneMapper::operator()(const glm::vec3& src) const noexcept {
	// Exposure tone mapping
	glm::vec3 mapped = glm::vec3(1.0) - glm::exp(-src * mExposure);

	// Gamma correction 
	return glm::pow(mapped, glm::vec3(1.0 / mGamma));
}