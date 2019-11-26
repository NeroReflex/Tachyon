#include "SpaceRequiringResource.h"
#include "MemoryPool.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

SpaceRequiringResource::SpaceRequiringResource() noexcept
	: mMemoryRequirements(nullptr), mUsedMemoryPool(nullptr), mStartingOffset(0) {}

SpaceRequiringResource::~SpaceRequiringResource() {
	if (mUsedMemoryPool) mUsedMemoryPool->free(mStartingOffset, *this);
}

const VkMemoryRequirements& SpaceRequiringResource::getMemoryRequirements() const noexcept
{
	if (!mMemoryRequirements) {
		auto result = queryMemoryRequirements().release();
		mMemoryRequirements.reset(result);
	}
	
	return *(mMemoryRequirements.get());
}

void SpaceRequiringResource::execBinding(const Device* const device, MemoryPool* memoryPool, VkDeviceSize offset) noexcept {
	mUsedMemoryPool = memoryPool;
	mStartingOffset = offset;
	bindMemory(device, memoryPool->getNativeDeviceMemoryHandle(), mStartingOffset);
}
