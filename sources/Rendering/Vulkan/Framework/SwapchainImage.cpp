#include "SwapchainImage.h"
#include "ImageView.h"
#include "Swapchain.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

SwapchainImage::SwapchainImage(const Device* device, const Swapchain* swapchain, VkImage&& image) noexcept
	: DeviceOwned(device),
	ImageInterface(device, std::move(image)),
	mParentSwapchain(swapchain) {

	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = getNativeImageHandle();
	createInfo.pNext = nullptr;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = getFormat();
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView swapchainImageView;
	vkCreateImageView(getParentSwapchain()->getParentDevice()->getNativeDeviceHandle(), &createInfo, nullptr, &swapchainImageView);

	mImageView.reset(new ImageView(this, std::move(swapchainImageView)));
}

SwapchainImage::~SwapchainImage() {}

const Swapchain* SwapchainImage::getParentSwapchain() const noexcept {
	return mParentSwapchain;
}

ImageView* SwapchainImage::getImageView() const noexcept {
	return mImageView.get();
}

const VkFormat& SwapchainImage::getFormat() const noexcept {
	return getParentSwapchain()->getFormat();
}
