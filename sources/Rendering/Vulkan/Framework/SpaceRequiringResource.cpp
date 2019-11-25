#include "SpaceRequiringResource.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

SpaceRequiringResource::SpaceRequiringResource() noexcept
	: mAllocated(false) {}

void SpaceRequiringResource::setMemoryRequirements(VkMemoryRequirements&& memReq) noexcept {
	mMemoryRequirementsSet = true;

	std::swap(memReq, mMemoryRequirements);
}

SpaceRequiringResource::~SpaceRequiringResource() {
	DBG_ASSERT( (!mAllocated));
}

 uint32_t SpaceRequiringResource::getRequiredSpace() const noexcept {
	DBG_ASSERT(mMemoryRequirementsSet);

	return mMemoryRequirements.size;
}

uint32_t SpaceRequiringResource::getRequiredAlignment() const noexcept {
	DBG_ASSERT(mMemoryRequirementsSet);

	return mMemoryRequirements.alignment;
}

uint32_t SpaceRequiringResource::getRequiredMemoryTypes() const noexcept {
	return mMemoryRequirements.memoryTypeBits;
}

void SpaceRequiringResource::bindMemory(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset ) noexcept {
	DBG_ASSERT((!mAllocated));
	malloc(device, std::move(memoryPool), std::move(offset));
	mAllocated = true;
}

void SpaceRequiringResource::unbindMemory() noexcept {
	DBG_ASSERT(mAllocated);
	
	//TODO: unbind memory
	
	mAllocated = false;
}
