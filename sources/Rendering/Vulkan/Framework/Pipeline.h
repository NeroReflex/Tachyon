#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Pipeline :
					virtual public DeviceOwned {

				public:
					enum class PipelineType {
						Compute,
						Graphics,
					};

					Pipeline(const Device* device, PipelineType type, VkPipelineLayout&& pipelineLayout, VkDescriptorSetLayout&& descriptorSetLayout, VkPipeline&& pipeline) noexcept;

					Pipeline(const Pipeline&) = delete;

					Pipeline(Pipeline&&) = delete;

					Pipeline& operator=(const Pipeline&) = delete;

					~Pipeline() override;

					const VkPipeline& getNativePipelineHandle() const noexcept;

					const VkPipelineLayout& getNativePipelineLayoutHandle() const noexcept;

					const VkDescriptorSetLayout& getNativeDescriptorSetLayout() const noexcept;

				private:
					PipelineType mType;

					VkPipeline mPipeline;

					VkPipelineLayout mPipelineLayout;

					VkDescriptorSetLayout mDescriptorSetLayout;
				};
			}
		}
	}
}