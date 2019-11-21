#include "Device.h"
#include "Swapchain.h"
#include "Shader.h"

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

const Pipeline* Device::createGraphicPipeline(
	const Shader& vertexShader,

	const Shader& fragmentShader) const noexcept
{
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfo(2);
	shaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo[0].pNext = nullptr;
	shaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageInfo[0].module = vertexShader.getNativeShaderModuleHandle();
	shaderStageInfo[0].pName = "main";
	shaderStageInfo[0].pSpecializationInfo = nullptr;

	shaderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo[1].pNext = nullptr;
	shaderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageInfo[1].module = fragmentShader.getNativeShaderModuleHandle();
	shaderStageInfo[1].pName = "main";
	shaderStageInfo[1].pSpecializationInfo = nullptr;

	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	VK_CHECK_RESULT(vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));

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
