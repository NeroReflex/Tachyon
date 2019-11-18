#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Device::Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device) noexcept
	: InstanceOwned(instance),
	mPhysicalDevice(physicalDevice),
	mDevice(device) {}

Device::~Device() {
	vkDestroyDevice(mDevice, nullptr);
}

