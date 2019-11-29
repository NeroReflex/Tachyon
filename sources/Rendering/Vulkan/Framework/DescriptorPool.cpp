#include "DescriptorPool.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DescriptorPool::DescriptorPool(const Device* const device, VkDescriptorPool&& descriptorPool) noexcept
	: DeviceOwned(device),
	mDescriptorPool(std::move(descriptorPool)) {}

DescriptorPool::~DescriptorPool() {
	vkDestroyDescriptorPool(getParentDevice()->getNativeDeviceHandle(), mDescriptorPool, nullptr);
}

const VkDescriptorPool& DescriptorPool::getNativeDescriptorPoolHandle() const noexcept {
	return mDescriptorPool;
}
