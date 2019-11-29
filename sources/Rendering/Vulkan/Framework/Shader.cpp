#include "Shader.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Shader::Shader(const Device* device, ShaderType type, const ShaderLayoutBinding& bindings, const char* source, uint32_t size) noexcept
	: DeviceOwned(device),
	mShaderType(type),
	mSpecializedBindings(std::move(bindings.getNativeLayoutHandles())),
	mDescriptorPoolSize(std::move(bindings.getNativeDescriptorPoolSizeHandles())) {

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(source);
	createInfo.flags = 0; // At the time of writing reserved for future use

	VkShaderStageFlags shaderStageFlags;
	switch (type) {
	case ShaderType::Vertex:
		shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ShaderType::Fragment:
		shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case ShaderType::Geometry:
		shaderStageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case ShaderType::Compute:
		shaderStageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		DBG_ASSERT(false);
	}

	for (auto& binding: mSpecializedBindings) {
		binding.stageFlags = shaderStageFlags;
	}

	VK_CHECK_RESULT(vkCreateShaderModule(getParentDevice()->getNativeDeviceHandle(), &createInfo, nullptr, &mShaderModule));
}

Shader::~Shader() {
	vkDestroyShaderModule(getParentDevice()->getNativeDeviceHandle(), mShaderModule, nullptr);
}

const VkShaderModule& Shader::getNativeShaderModuleHandle() const noexcept {
	return mShaderModule;
}

const std::vector<VkDescriptorSetLayoutBinding>& Shader::getNativeShaderBindings() const noexcept {
	return mSpecializedBindings;
}

const Shader::ShaderType& Shader::getType() const noexcept {
	return mShaderType;
}

std::vector<VkDescriptorPoolSize> Shader::getNativeDescriptorPoolSizeHandles() const noexcept {
	return mDescriptorPoolSize;
}
