#include "Pipeline.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Pipeline::Pipeline(Device* device, PipelineType type, VkPipelineLayout&& pipelineLayout, VkDescriptorSetLayout&& descriptorSetLayout, VkPipeline&& pipeline) noexcept
	: DeviceOwned(device),
	mType(type),
	mPipeline(std::move(pipeline)),
	mPipelineLayout(std::move(pipelineLayout)),
	mDescriptorSetLayout(descriptorSetLayout) {}

Pipeline::~Pipeline() {
	vkDestroyDescriptorSetLayout(getParentDevice()->getNativeDeviceHandle(), mDescriptorSetLayout, NULL);

	vkDestroyPipelineLayout(getParentDevice()->getNativeDeviceHandle(), mPipelineLayout, nullptr);

	vkDestroyPipeline(getParentDevice()->getNativeDeviceHandle(), mPipeline, nullptr);
}

const VkPipeline& Pipeline::getNativePipelineHandle() const noexcept {
	return mPipeline;
}

const VkPipelineLayout& Pipeline::getNativePipelineLayoutHandle() const noexcept {
	return mPipelineLayout;
}

const VkDescriptorSetLayout& Pipeline::getNativeDescriptorSetLayout() const noexcept {
	return mDescriptorSetLayout;
}
