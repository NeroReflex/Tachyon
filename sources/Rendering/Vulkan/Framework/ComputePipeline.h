#pragma once

#include "Pipeline.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class ComputePipeline :
					public Pipeline {

				public:
					inline ComputePipeline(const Device* device, VkPipeline&& pipeline) noexcept
						: DeviceOwned(device),
						Pipeline(device, Pipeline::PipelineType::Compute, std::move(pipeline)) {};

					ComputePipeline(const ComputePipeline&) = delete;

					ComputePipeline(ComputePipeline&&) = delete;

					ComputePipeline& operator=(const Pipeline&) = delete;

					~ComputePipeline() override = default;
				};
			}
		}
	}
}