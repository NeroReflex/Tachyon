#include "Fence.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Fence::Fence(const Device* const device, VkFence&& fence) noexcept
	: DeviceOwned(device), mFence(std::move(fence)) {}

Fence::~Fence() {
	vkDestroyFence(getParentDevice()->getNativeDeviceHandle(), mFence, nullptr);
}
