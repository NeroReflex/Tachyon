#pragma once

#include "ShaderLayoutBinding.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class DescriptorPool;

				class DescriptorSet {

				public:
					DescriptorSet(DescriptorPool* const pool, VkDescriptorSet&& descriptorPool) noexcept;

					DescriptorSet(const DescriptorSet&) = delete;

					DescriptorSet(DescriptorSet&&) = delete;

					DescriptorSet& operator=(const DescriptorSet&) = delete;

					~DescriptorSet();

					DescriptorPool* getParentDescriptorPool() const noexcept;

					const VkDescriptorSet& getNativeDescriptorSetHandle() const noexcept;

				private:
					DescriptorPool* const mParentDescriptorPool;

					VkDescriptorSet mDescriptorSet;
				};
			}
		}
	}
}
