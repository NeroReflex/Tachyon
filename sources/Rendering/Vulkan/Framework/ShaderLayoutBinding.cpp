#include "ShaderLayoutBinding.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

ShaderLayoutBinding::ShaderLayoutBinding(const std::initializer_list<BindingDescriptor>& descriptors) noexcept
	: mDescriptors() {
	for (const auto& desc : descriptors)
		insert(desc);
}

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

void ShaderLayoutBinding::insert(const BindingDescriptor& shaderBinding) noexcept {
	VkDescriptorType type;

	switch (shaderBinding.bindingType) {
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
	binding.binding = shaderBinding.bindingPoint;
	binding.descriptorType = type;
	binding.descriptorCount = shaderBinding.count;
	binding.pImmutableSamplers = nullptr;

	mDescriptors.emplace_back(binding);

	auto it = mDescriptorPoolSize.find(type);
	if (it == mDescriptorPoolSize.end()) {
		mDescriptorPoolSize.emplace(std::pair<VkDescriptorType, uint32_t>(type, 1));
	} else {
		*(it)++;
	}
}

std::vector<VkDescriptorSetLayoutBinding> ShaderLayoutBinding::getNativeLayoutHandles() const noexcept {
	return mDescriptors;
}

std::vector<VkDescriptorPoolSize> ShaderLayoutBinding::join(const std::vector<VkDescriptorPoolSize>& src1, const std::vector<VkDescriptorPoolSize>& src2) noexcept {
	std::vector<VkDescriptorPoolSize> result(src1);

	for (uint32_t i = 0; i < src2.size(); ++i) {
		bool done = false;
		for (uint32_t j = 0; (j < result.size()) && (!done); ++j) {
			if (result[j].type == src2[i].type) {
				result[j].descriptorCount;
				done = true;
			}
		}

		if (!done) {
			VkDescriptorPoolSize el;
			el.type = src2[i].type;
			el.descriptorCount = 1;
			result.push_back(el);
		}
	}

	return result;
}

std::vector<VkDescriptorPoolSize> ShaderLayoutBinding::getNativeDescriptorPoolSizeHandles() const noexcept {
	std::vector<VkDescriptorPoolSize> result;

	for (const auto& poolSize : mDescriptorPoolSize) {
		VkDescriptorPoolSize el;
		el.type = poolSize.first;
		el.descriptorCount = poolSize.second;
		
		result.push_back(el);
	}

	return result;
}