#include "SpaceRequiringResource.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

void SpaceRequiringResource::setMemoryRequirements(VkMemoryRequirements&& memReq) noexcept {
	mMemoryRequirementsSet = true;

	std::swap(memReq, mMemoryRequirements);
}

const uint32_t& SpaceRequiringResource::getRequiredSpace() const noexcept {
	DBG_ASSERT(mMemoryRequirementsSet);

	return mMemoryRequirements.size;
}

const uint32_t& SpaceRequiringResource::getRequiredAlignment() const noexcept {
	DBG_ASSERT(mMemoryRequirementsSet);

	return mMemoryRequirements.alignment;
}