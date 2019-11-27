#include "CommandBuffer.h"
#include "CommandPool.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

CommandBuffer::CommandBuffer(const CommandPool* const parentCommandPool, VkCommandBuffer&& commandBuffer) noexcept
	: mParentCommandPool(parentCommandPool),
	mCommandBuffer(commandBuffer) {}

CommandBuffer::~CommandBuffer() {}

