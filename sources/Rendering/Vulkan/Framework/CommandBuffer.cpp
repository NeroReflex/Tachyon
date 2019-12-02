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

void CommandBuffer::registerCommands(std::function<void(const VkCommandBuffer& commandBuffer)> fn) noexcept {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only submitted and used once in this application.
	VK_CHECK_RESULT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo)); // start recording commands.

	fn(mCommandBuffer);

	/*
	//We need to bind a pipeline, AND a descriptor set before we dispatch.
	//The validation layer will NOT give warnings if you forget these, so be very careful not to forget them.
	
	vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

	
	//Calling vkCmdDispatch basically starts the compute pipeline, and executes the compute shader.
	//The number of workgroups is specified in the arguments.
	//If you are already familiar with compute shaders from OpenGL, this should be nothing new to you.
	
	vkCmdDispatch(mCommandBuffer, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(HEIGHT / float(WORKGROUP_SIZE)), 1);
	*/

	VK_CHECK_RESULT(vkEndCommandBuffer(mCommandBuffer)); // end recording commands.
}

void CommandBuffer::submit(Fence* fence) noexcept
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffer;

	//VK_CHECK_RESULT(vkQueueSubmit());
}
