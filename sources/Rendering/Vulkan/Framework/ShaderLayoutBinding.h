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

					struct BindingDescriptor {
						BindingType bindingType;
						uint32_t bindingPoint;
						uint32_t count;

						/**
						 * Create a binding descriptor.
						 *
						 * @param bindingType the type of the binding
						 * @param binding point the layout location
						 * @param count the number of bindings accessed as an array or the number of bytes for a uniform buffer
						 */
						inline BindingDescriptor(BindingType bindingType, uint32_t bindingPoint, uint32_t count) noexcept
							: bindingType(bindingType), bindingPoint(bindingPoint), count(count) {};
					};

					ShaderLayoutBinding(const std::initializer_list<BindingDescriptor>& descriptors = std::initializer_list<BindingDescriptor>()) noexcept;

					ShaderLayoutBinding(const ShaderLayoutBinding&) noexcept;

					ShaderLayoutBinding(ShaderLayoutBinding&&) noexcept;

					ShaderLayoutBinding& operator=(const ShaderLayoutBinding&) noexcept;

					~ShaderLayoutBinding();
					
					void insert(const BindingDescriptor& binding) noexcept;

					std::vector<VkDescriptorSetLayoutBinding> getNativeLayoutHandles() const noexcept;

					std::vector<VkDescriptorPoolSize> getNativeDescriptorPoolSizeHandles() const noexcept;

					static std::vector<VkDescriptorPoolSize> join(const std::vector<VkDescriptorPoolSize>& src1, const std::vector<VkDescriptorPoolSize>& src2) noexcept;

				private:
					std::vector<VkDescriptorSetLayoutBinding> mDescriptors;

					std::unordered_map<VkDescriptorType, uint32_t> mDescriptorPoolSize;
				};
			}
		}
	}
}
