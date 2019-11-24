#include "HDR.h"

using namespace Tachyon;
using namespace Tachyon::Core;

const glm::float32 HDR::defaultGamma = static_cast<glm::float32>(2.2);
const glm::float32 HDR::defaultExposure = static_cast<glm::float32>(0.1);

HDR::HDR(glm::float32 gamma, glm::float32 exposure) noexcept
	: mGamma(std::move(gamma)), mExposure(std::move(exposure)) {

	// Instances of this class are going to be memcpy'ed into GPU memory
	static_assert(sizeof(HDR) == 8, "HDR class has a strange layout");
}