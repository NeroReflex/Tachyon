#include "ShaderLayoutBinding.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

ShaderLayoutBinding::ShaderLayoutBinding() noexcept
	: mDescriptors() {}

ShaderLayoutBinding::ShaderLayoutBinding(const ShaderLayoutBinding& src) noexcept
	: mDescriptors(src.mDescriptors) {}

ShaderLayoutBinding::ShaderLayoutBinding(ShaderLayoutBinding&& src) noexcept
	: mDescriptors(std::move(src.mDescriptors)) {}

ShaderLayoutBinding& ShaderLayoutBinding::operator=(const ShaderLayoutBinding& src) noexcept {
	if (this != &src) {
		mDescriptors = src.mDescriptors;
	}

	return *this;
}

ShaderLayoutBinding::~ShaderLayoutBinding() {}

void ShaderLayoutBinding::insert(BindingType bindingType, uint32_t bindingPoint, uint32_t count) noexcept {
	VkDescriptorType type;

	switch (bindingType) {
	case BindingType::Sampler:
		type = VK_DESCRIPTOR_TYPE_SAMPLER;
		break;
	case BindingType::SampledImage:
		type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		break;
	case BindingType::CombinedImageSampler:
		type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	case BindingType::StorageImage:
		type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		break;
	case BindingType::UniformTexelStorage:
		type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		break;
	case BindingType::UniformTexelBuffer:
		type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		break;
	case BindingType::UniformBuffer:
		type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	case BindingType::StorageBuffer:
		type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		break;
	default:
		DBG_ASSERT(false);
		break;
	}

	VkDescriptorSetLayoutBinding binding;
	binding.binding = bindingPoint;
	binding.descriptorType = type;
	binding.descriptorCount = count;
	binding.pImmutableSamplers = nullptr;

	mDescriptors.emplace_back(binding);
}

std::vector<VkDescriptorSetLayoutBinding> ShaderLayoutBinding::getNativeLayoutHandles() const noexcept {
	return mDescriptors;
}