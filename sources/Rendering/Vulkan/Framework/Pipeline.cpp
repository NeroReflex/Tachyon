#include "Pipeline.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Pipeline::Pipeline(const Device* device, PipelineType type, VkPipelineLayout&& pipelineLayout, VkPipeline&& pipeline) noexcept
	: DeviceOwned(device),
	mType(type),
	mPipeline(std::move(pipeline)),
	mPipelineLayout(std::move(pipelineLayout)) {}

Pipeline::~Pipeline() {
	vkDestroyPipelineLayout(getParentDevice()->getNativeDeviceHandle(), mPipelineLayout, nullptr);

	vkDestroyPipeline(getParentDevice()->getNativeDeviceHandle(), mPipeline, nullptr);
}

const VkPipeline& Pipeline::getNativePipelineHandle() const noexcept {
	return mPipeline;
}

const VkPipelineLayout& Pipeline::getNativePipelineLayoutHandle() const noexcept {
	return mPipelineLayout;
}
