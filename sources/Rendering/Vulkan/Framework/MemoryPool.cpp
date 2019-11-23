#include "MemoryPool.h"
#include "Device.h"
#include "SpaceRequiringResource.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

MemoryPool::MemoryPool(const Device* device, VkDeviceSize totalSize, VkDeviceMemory&& memory) noexcept
	: DeviceOwned(device),
	mDeviceMemory(std::move(memory)),
	mTotalSize(totalSize) {}

MemoryPool::~MemoryPool() {
	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}

