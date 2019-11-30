#pragma once

#include "ShaderLayoutBinding.h"
#include "ImageInterface.h"

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

					void bindBuffer() const noexcept;

					/**
					 * Update a set of contiguous image bindings starting from the given binding point.
					 *
					 * @param firstLayoutId the number of the fisrst layout
					 * @param images images to bind from the first binding point onward
					 */
					void bindImages(uint32_t firstLayoutId, std::initializer_list<const ImageInterface*> images) const noexcept;

				private:
					DescriptorPool* const mParentDescriptorPool;

					VkDescriptorSet mDescriptorSet;
				};
			}
		}
	}
}
