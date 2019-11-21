#include "Pipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Pipeline::Pipeline(const Device* device, VkPipeline&& pipeline) noexcept
	: DeviceOwned(device),
	mPipeline(std::move(pipeline)) {}

Pipeline::~Pipeline() {}

const VkPipeline& Pipeline::getNativePipelineHandle() const noexcept {
	return mPipeline;
}