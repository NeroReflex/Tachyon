#include "HDR.h"

using namespace Tachyon;
using namespace Tachyon::Core;

HDR::HDR(glm::float32 gamma, glm::float32 exposure) noexcept
	: mGamma(std::move(gamma)), mExposure(std::move(exposure)) {

	// Instances of this class are going to be memcpy'ed into GPU memory
	static_assert(sizeof(HDR) == 8, "HDR class has a strange layout");
}