#pragma once

#include "Pipeline.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class ComputePipeline :
					public Pipeline {

				public:
					inline ComputePipeline(const Device* device, VkPipelineLayout&& pipelineLayout, VkDescriptorSetLayout&& descriptorSetLayout, VkPipeline&& pipeline) noexcept
						: DeviceOwned(device), Pipeline(device, Pipeline::PipelineType::Compute, std::move(pipelineLayout), std::move(descriptorSetLayout), std::move(pipeline)) {};

					ComputePipeline(const ComputePipeline&) = delete;

					ComputePipeline(ComputePipeline&&) = delete;

					ComputePipeline& operator=(const Pipeline&) = delete;

					~ComputePipeline() override = default;
				};
			}
		}
	}
}