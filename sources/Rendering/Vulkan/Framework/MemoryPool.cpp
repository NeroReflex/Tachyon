#include "MemoryPool.h"
#include "Device.h"
#include "SpaceRequiringResource.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

MemoryPool::MemoryPool(const Device* device, const std::initializer_list<const SpaceRequiringResource*>& resources) noexcept
	: DeviceOwned(device) {
	
	DBG_ASSERT((resources.size() > 0));
	VkDeviceSize globalBufferSize = 0;
	for (const auto& allocRequiringResource : resources) {
		globalBufferSize += static_cast<VkDeviceSize>(allocRequiringResource->getRequiredAlignment()) + static_cast<VkDeviceSize>(allocRequiringResource->getRequiredSpace());
	}

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = globalBufferSize;
	//allocateInfo.memoryTypeIndex = findMemoryType(raytracerTLASMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Allocate a big chunk of memory on the device
	VK_CHECK_RESULT(vkAllocateMemory(getParentDevice()->getNativeDeviceHandle(), &allocateInfo, NULL, &mDeviceMemory));
}

MemoryPool::~MemoryPool() {
	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}