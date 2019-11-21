#include "Shader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;
using namespace Tachyon::Rendering::Vulkan::Framework::Shaders;

Shader::Shader(const Device* device, char* source, uint32_t size) noexcept
	: DeviceOwned(device) {
	
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(source);

	VK_CHECK_RESULT(vkCreateShaderModule(getParentDevice()->getNativeDeviceHandle(), &createInfo, nullptr, &mShaderModule));
}

Shader::~Shader() {
	vkDestroyShaderModule(getParentDevice()->getNativeDeviceHandle(), mShaderModule, nullptr);
}

const VkShaderModule& Shader::getNativeShaderModuleHandle() const noexcept {
	return mShaderModule;
}
