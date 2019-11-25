#include "MemoryPool.h"
#include "Device.h"
#include "SpaceRequiringResource.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

MemoryPool::MemoryPool(const Device* device, VkMemoryPropertyFlagBits props, uint32_t memoryTypeBits, VkDeviceSize pagesCount, VkDeviceMemory&& memory) noexcept
	: DeviceOwned(device),
	mProperties(std::move(props)),
	mMemoryTypeBits(memoryTypeBits),
	mDeviceMemory(std::move(memory)),
	mFixedPageTracker(new uint32_t[Memory::PoolManager::getNumberOfPagesUsedToManagePages(pagesCount)]),
	mPoolManager(pagesCount, NULL, mFixedPageTracker, true) {}

MemoryPool::~MemoryPool() {
	delete[] mFixedPageTracker;

	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}

const VkMemoryPropertyFlagBits& MemoryPool::getMemoryProperties() const noexcept {
	return mProperties;
}

const VkDeviceMemory& MemoryPool::getNativeDeviceMemoryHandle() const noexcept {
	return mDeviceMemory;
}

VkDeviceSize MemoryPool::malloc(const SpaceRequiringResource& resource, VkDeviceSize hint) noexcept {
	const auto result = mPoolManager.malloc(resource.getRequiredSpace(), resource.getRequiredAlignment(), (void*)(uintptr_t(hint)));
	DBG_ASSERT(result.success);
	return uintptr_t(result.result);
}



