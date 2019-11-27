#include "CommandPool.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

CommandPool::CommandPool(const Device* const device, VkCommandPool&& commandPool) noexcept 
	: DeviceOwned(device),
	mCommandPool(std::move(commandPool)) {}

CommandPool::~CommandPool() {
	vkDestroyCommandPool(getParentDevice()->getNativeDeviceHandle(), mCommandPool, nullptr);
}

void CommandPool::registerCommands(std::function<void()> fn) noexcept {
	
}
