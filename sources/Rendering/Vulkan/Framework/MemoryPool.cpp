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
	mFixedPageTracker(::malloc(Memory::UnsafePoolManager::getManagementReservedSpace(pagesCount))),
	mPoolManager(pagesCount, NULL, mFixedPageTracker) {}

MemoryPool::~MemoryPool() {
	::free(mFixedPageTracker);

	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}

const VkMemoryPropertyFlagBits& MemoryPool::getMemoryProperties() const noexcept {
	return mProperties;
}

const VkDeviceMemory& MemoryPool::getNativeDeviceMemoryHandle() const noexcept {
	return mDeviceMemory;
}

VkDeviceSize MemoryPool::malloc(const SpaceRequiringResource& resource, VkDeviceSize hint) noexcept {
	const auto requirements = resource.getMemoryRequirements();

	const auto result = mPoolManager.malloc(requirements.size, requirements.alignment);
	DBG_ASSERT(result.success);
	return uintptr_t(result.result);
}

void MemoryPool::free(VkDeviceSize ptr, const SpaceRequiringResource& resource) noexcept
{
	const auto requirements = resource.getMemoryRequirements();
	mPoolManager.free((void*)(uintptr_t(ptr)), requirements.size, requirements.alignment);
}



