#include "ReinhardToneMapping.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

ReinhardToneMapping::ReinhardToneMapping() noexcept
	: mExposure(std::move(exposure)) {}

glm::vec3 ReinhardToneMapping::operator()(const glm::vec3& src) noexcept {
	const float gamma = 2.2;
	glm::vec3 hdrColor = src;

	// reinhard tone mapping
	glm::vec3 mapped = hdrColor / (hdrColor + glm::vec3(1.0));
	
	// gamma correction 
	mapped = glm::pow(mapped, glm::vec3(1.0 / gamma));

	return mapped;
}