#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

Device::Device(const std::shared_ptr<Instance>& instance, VkDevice&& device, VkQueue&& initialQueue) noexcept
	: InstanceObject(instance),
	mDevice(std::move(device)),
	mQueueCollection() {
	mQueueCollection.emplace_back(std::move(initialQueue));
}

Device::~Device() {
	vkDestroyDevice(mDevice, nullptr);
}

const VkDevice& Device::getNativeHandle() const noexcept {
	return mDevice;
}