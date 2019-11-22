#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

VkPresentModeKHR Device::SwapchainSelector::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept {
	for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR Device::SwapchainSelector::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkSwapchainCreateInfoKHR Device::SwapchainSelector::operator()(const SwapChainSupportDetails& swapChainSupport) const noexcept {
	VkSwapchainCreateInfoKHR createInfo;

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    	imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.presentMode = presentMode;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;

	return createInfo;
}

Device::Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device, uint32_t queueFamilyIndex) noexcept
	: InstanceOwned(instance),
	mPhysicalDevice(physicalDevice),
	mDevice(device),
	mQueueFamilyIndex(queueFamilyIndex) {
	// Query avaliable extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr);
	
	mAvailableExtensions.resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, mAvailableExtensions.data());

	// Query swapchain support
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &mSupportedSwapchain.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, getParentInstance()->getSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		mSupportedSwapchain.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, getParentInstance()->getSurface(), &formatCount, mSupportedSwapchain.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		mSupportedSwapchain.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &presentModeCount, mSupportedSwapchain.presentModes.data());
	}

	DBG_ASSERT((isExtensionAvailable(VK_KHR_SWAPCHAIN_EXTENSION_NAME)));

	DBG_ASSERT(((!mSupportedSwapchain.formats.empty()) && (!mSupportedSwapchain.presentModes.empty())));
	
}

Device::~Device() {
	mSwapchain.reset();

	vkDestroyDevice(mDevice, nullptr);
}

bool Device::isExtensionAvailable(const std::string& extName) const noexcept {
	for (const auto& extension : mAvailableExtensions) {
		if (strcmp(extName.c_str(), extension.extensionName) == 0) {
			return true;
		}
	}

	return false;
}

const VkDevice& Tachyon::Rendering::Vulkan::Framework::Device::getNativeDeviceHandle() const noexcept
{
	return mDevice;
}

const ComputeShader* Device::loadComputeShader(const ShaderLayoutBinding& bindings, const char* source, uint32_t size) const noexcept
{
	return new ComputeShader(this, bindings, source, size);
}

const Pipeline* Device::createPipeline(const std::vector<const Shader*>& shaders) const noexcept
{
	DBG_ASSERT( (shaders.size() > 0) );
	bool isComputePipeline = (shaders.size() == 1) && (shaders[0]->getType() == Shader::ShaderType::Compute);

	std::vector<VkPipelineShaderStageCreateInfo> shadersStageInfo(shaders.size());
	std::vector<VkDescriptorSetLayoutBinding> renderDescriptorSetLayoutBinding;
	uint32_t i = 0;
	for (const auto& shader : shaders) {
		VkShaderStageFlagBits stageType;

		switch (shader->getType()) {
		case Shader::ShaderType::Vertex:
			stageType = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case Shader::ShaderType::Fragment:
			stageType = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case Shader::ShaderType::Geometry:
			stageType = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case Shader::ShaderType::Compute:
			stageType = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		default:
			DBG_ASSERT(false);
		}

		shadersStageInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shadersStageInfo[i].pNext = nullptr;
		shadersStageInfo[i].stage = stageType;
		shadersStageInfo[i].module = shader->getNativeShaderModuleHandle();
		shadersStageInfo[i].pName = "main";
		shadersStageInfo[i].pSpecializationInfo = nullptr;

		const auto& shaderBindings = shader->getNativeShaderBindings();
		renderDescriptorSetLayoutBinding.insert(renderDescriptorSetLayoutBinding.cend(), shaderBindings.cbegin(), shaderBindings.cend());

		++i;
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = NULL;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(renderDescriptorSetLayoutBinding.size()); // only a single binding in this descriptor set layout. 
	descriptorSetLayoutCreateInfo.pBindings = renderDescriptorSetLayoutBinding.data();

	VkDescriptorSetLayout renderDescriptorSetLayout;
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, NULL, &renderDescriptorSetLayout));
	
	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &renderDescriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

	VK_CHECK_RESULT(vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

	VkPipeline nativePipeline;
	if (isComputePipeline) {
		VkComputePipelineCreateInfo computePipelineCreateInfo;
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
		computePipelineCreateInfo.flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif
		computePipelineCreateInfo.stage = shadersStageInfo[0];
		computePipelineCreateInfo.layout = pipelineLayout;
		VK_CHECK_RESULT(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, NULL, &nativePipeline));
		
	} else {
		DBG_ASSERT(false);
	}

	//vkDestroyDescriptorSetLayout(mDevice, *flushDescriptorSetLayout, NULL);

	return nullptr;
}

Swapchain* Device::createSwapchain(uint32_t width, uint32_t height, const SwapchainSelector& selector) noexcept {
	DBG_ASSERT( (!mSwapchain) );

	VkExtent2D actualExtent = { width, height };

	VkSwapchainCreateInfoKHR createInfo = selector(mSupportedSwapchain);
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	createInfo.surface = getParentInstance()->getSurface();
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
	actualExtent.width = std::max(mSupportedSwapchain.capabilities.minImageExtent.width, std::min(mSupportedSwapchain.capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(mSupportedSwapchain.capabilities.minImageExtent.height, std::min(mSupportedSwapchain.capabilities.maxImageExtent.height, actualExtent.height));
	createInfo.imageExtent = actualExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR /*VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR*/;
	createInfo.clipped = VK_TRUE;
	createInfo.flags = 0;
	
	VkBool32 isSurfaceSupported = VK_FALSE;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, mQueueFamilyIndex, getParentInstance()->getSurface(), &isSurfaceSupported));
	DBG_ASSERT((isSurfaceSupported == true));

	VkSwapchainKHR swapchain;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &swapchain));

	mSwapchain.reset(new Swapchain(this, std::move(swapchain), createInfo.imageFormat, actualExtent.width, actualExtent.height));
	return mSwapchain.get();
}

Swapchain* Device::getSwapchain() const noexcept
{
	return mSwapchain.get();
}
