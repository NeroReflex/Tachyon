#include "ExposureToneMapper.h"

using namespace Tachyon;
using namespace Tachyon::Rendering::CPU;
using namespace Tachyon::Rendering::CPU::ToneMapping;

ExposureToneMapper::ExposureToneMapper(glm::float32 exposure) noexcept
	: mExposure(std::move(exposure)) {}

glm::vec3 ExposureToneMapper::operator()(const glm::vec3& src) noexcept {
	const float gamma = 2.2;
	glm::vec3 hdrColor = src;

	// Exposure tone mapping
	glm::vec3 mapped = glm::vec3(1.0) - glm::exp(-hdrColor * mExposure);

	// Gamma correction 
	return glm::pow(mapped, glm::vec3(1.0 / gamma));
}