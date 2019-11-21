#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class ShaderLayoutBinding {

				public:
					enum class BindingType {
						Sampler,
						SampledImage,
						CombinedImageSampler,
						StorageImage,
						UniformTexelStorage,
						UniformTexelBuffer,
						UniformBuffer,
						StorageBuffer,
					};

					ShaderLayoutBinding() noexcept;

					ShaderLayoutBinding(const ShaderLayoutBinding&) noexcept;

					ShaderLayoutBinding(ShaderLayoutBinding&&) noexcept;

					ShaderLayoutBinding& operator=(const ShaderLayoutBinding&) noexcept;

					~ShaderLayoutBinding();

					/**
					 *
					 *
					 * @count the number of bindings accessed as an array or the number of bytes for a uniform buffer
					 */
					void insert(BindingType bindingType, uint32_t bindingPoint, uint32_t count) noexcept;

					std::vector<VkDescriptorSetLayoutBinding> getNativeLayoutHandles() const noexcept;

				private:
					std::vector<VkDescriptorSetLayoutBinding> mDescriptors;
				};
			}
		}
	}
}
