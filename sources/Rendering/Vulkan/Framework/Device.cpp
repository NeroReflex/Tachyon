#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

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
