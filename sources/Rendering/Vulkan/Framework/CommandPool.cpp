#include "CommandPool.h"
#include "Device.h"
#include "CommandBuffer.h"

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

const VkCommandPool& CommandPool::getNativeCommandPoolHandle() const noexcept {
	return mCommandPool;
}

CommandBuffer* CommandPool::createCommandBuffer() noexcept {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = mCommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1; // allocate a single command buffer. 

	VkCommandBuffer cmdBuffer;
	VK_CHECK_RESULT(vkAllocateCommandBuffers(getParentDevice()->getNativeDeviceHandle(), &commandBufferAllocateInfo, &cmdBuffer)); // allocate command buffer.

	CommandBuffer* buffer = new CommandBuffer(this, std::move(cmdBuffer));

	mRegisteredCommandBuffers.emplace_back(buffer);

	return buffer;
}
