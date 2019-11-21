#include "ImageView.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

ImageView::ImageView(const ImageInterface* image, VkImageView&& imageView) noexcept
	: mParentImage(image),
	mImageView(std::move(imageView)) {}

ImageView::~ImageView() {
	vkDestroyImageView(getParentImage()->getParentDevice()->getNativeDeviceHandle(), mImageView, nullptr);
}

const ImageInterface* ImageView::getParentImage() const noexcept {
	return mParentImage;
}

const VkImageView& ImageView::getNativeImageViewHandle() const noexcept {
	return mImageView;
}
