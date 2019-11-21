#include "Image.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Image::Image(const Device* device, bool swapchainImage, VkImage&& image) noexcept
	: DeviceOwned(device),
	mIsSwapchainImage(swapchainImage),
	mImage(std::move(image)) {}

Image::~Image() {
	if (!mIsSwapchainImage) vkDestroyImage(getParentDevice()->getNativeDeviceHandle(), mImage, nullptr);
}

const VkImage& Image::getNativeImageHandle() const noexcept {
	return mImage;
}
