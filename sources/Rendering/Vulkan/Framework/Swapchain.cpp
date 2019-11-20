#include "Swapchain.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Swapchain::Swapchain(const Device* device, VkSwapchainKHR&& swapchain, uint32_t width, uint32_t height) noexcept
    : DeviceOwned(device),
	DeviceMemoryBuffer(device),
    mSwapchain(swapchain),
	mWidth(width),
	mHeight(height) {}

Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, nullptr);
}

