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
	mHeight(height) {

	uint32_t framebuffersCount;
	vkGetSwapchainImagesKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, &framebuffersCount, nullptr);

	std::vector<VkImage> framebuffers(framebuffersCount);
	vkGetSwapchainImagesKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, &framebuffersCount, framebuffers.data());

	for (size_t i = 0; i < framebuffers.size(); ++i)
		mFramebuffers.emplace_back(new SwapchainImage(getParentDevice(), std::move(framebuffers[i])));
	
}

Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, nullptr);
}
