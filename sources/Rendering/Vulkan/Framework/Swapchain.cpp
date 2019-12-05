#include "Swapchain.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Swapchain::Swapchain(Device* device, VkSwapchainKHR&& swapchain, VkFormat format, uint32_t width, uint32_t height) noexcept
    : DeviceOwned(device),
	DeviceMemoryBuffer(device),
    mSwapchain(swapchain),
	mFormat(format),
	mWidth(width),
	mHeight(height) {

	uint32_t framebuffersCount;
	vkGetSwapchainImagesKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, &framebuffersCount, nullptr);

	std::vector<VkImage> framebuffers(framebuffersCount);
	vkGetSwapchainImagesKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, &framebuffersCount, framebuffers.data());

	for (size_t i = 0; i < framebuffers.size(); ++i)
		mFramebuffers.emplace_back(new SwapchainImage(getParentDevice(), this, std::move(framebuffers[i])));
	
}

Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(getParentDevice()->getNativeDeviceHandle(), mSwapchain, nullptr);
}

const uint32_t& Swapchain::getWidth() const noexcept {
	return mWidth;
}

const uint32_t& Swapchain::getHeight() const noexcept {
	return mHeight;
}

const VkFormat& Swapchain::getFormat() const noexcept {
	return mFormat;
}
