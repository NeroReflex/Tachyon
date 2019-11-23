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
	uint32_t memoryTypeBits = 0xFFFFFFFF;
	VkDeviceSize globalBufferSize = 0;
	for (const auto& allocRequiringResource : resources) {
		globalBufferSize += static_cast<VkDeviceSize>(allocRequiringResource->getRequiredAlignment()) + static_cast<VkDeviceSize>(allocRequiringResource->getRequiredSpace());
		memoryTypeBits &= allocRequiringResource->getRequiredMemoryTypes();
	}

	DBG_ASSERT((memoryTypeBits != 0));

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = globalBufferSize;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Allocate a big chunk of memory on the device
	VK_CHECK_RESULT(vkAllocateMemory(getParentDevice()->getNativeDeviceHandle(), &allocateInfo, NULL, &mDeviceMemory));

	//TODO: register resources on this memory!!!
}

MemoryPool::~MemoryPool() {
	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}

uint32_t MemoryPool::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const noexcept {
	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceMemoryProperties(getParentDevice()->getNativePhysicalDeviceInstance(), &memoryProperties);

	/*
	How does this search work?
	See the documentation of VkPhysicalDeviceMemoryProperties for a detailed description.
	*/
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((memoryTypeBits & (1 << i)) &&
			((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}
	return -1;
}
