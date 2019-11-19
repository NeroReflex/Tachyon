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

VkSwapchainCreateInfoKHR Device::SwapchainSelector::operator()(const SwapChainSupportDetails& swapChainSupport) const noexcept {
	VkSwapchainCreateInfoKHR createInfo;

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    	imageCount = swapChainSupport.capabilities.maxImageCount;
	}


	

	return createInfo;
}

Device::Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device) noexcept
	: InstanceOwned(instance),
	mPhysicalDevice(physicalDevice),
	mDevice(device) {
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

Swapchain* Device::createSwapchain(const SwapchainSelector& selector) noexcept {
	DBG_ASSERT( (!mSwapchain) );

	VkSwapchainCreateInfoKHR createInfo;

	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = getParentInstance()->getSurface();

	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional


	mSwapchain.reset(nullptr);

	return mSwapchain.get();
}