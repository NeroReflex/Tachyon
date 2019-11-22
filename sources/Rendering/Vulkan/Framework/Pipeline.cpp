#include "Pipeline.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Pipeline::Pipeline(const Device* device, PipelineType type, VkPipeline&& pipeline) noexcept
	: DeviceOwned(device),
	mType(type),
	mPipeline(std::move(pipeline)) {}

Pipeline::~Pipeline() {
	vkDestroyPipeline(getParentDevice()->getNativeDeviceHandle(), mPipeline, nullptr);
}

const VkPipeline& Pipeline::getNativePipelineHandle() const noexcept {
	return mPipeline;
}