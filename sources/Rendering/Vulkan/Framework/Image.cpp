#include "Image.h"
#include "ImageView.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Image::Image(const Device* device, VkImage&& image) noexcept
	: DeviceOwned(device),
	ImageInterface(device, std::move(image)) {}

    
Image::~Image() {}


ImageView* Image::createImageView(ViewType type, VkFormat format, VkImageAspectFlagBits subrangeAspectBits, ViewColorMapping swizzle, uint32_t subrangeBaseMipLevel, uint32_t subrangeLevelCount, uint32_t subrangeBaseArrayLayer, uint32_t subrangeLayerCount) noexcept {
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.image = getNativeImageHandle();
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = subrangeAspectBits;
	createInfo.subresourceRange.baseMipLevel = subrangeBaseMipLevel;
	createInfo.subresourceRange.levelCount = subrangeLevelCount;
	createInfo.subresourceRange.baseArrayLayer = subrangeBaseArrayLayer;
	createInfo.subresourceRange.layerCount = subrangeLayerCount;

	switch (type) {
	case ViewType::Image1D:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
		break;

	case ViewType::Image2D:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		break;

	case ViewType::Image3D:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
		break;

	case ViewType::CubeMap:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		break;

	case ViewType::Image1DArray:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		break;

	case ViewType::Image2DArray:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		break;

	case ViewType::CubeMapArray:
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		break;

	default:
		DBG_ASSERT(false);
		break;
	}

	switch (swizzle) {
	case ViewColorMapping::rgba_rgba:
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		break;

	default:
		DBG_ASSERT(false);
		break;
	}

	VkImageView imageView;
	VK_CHECK_RESULT(vkCreateImageView(getParentDevice()->getNativeDeviceHandle(), &createInfo, nullptr, &imageView));

	ImageView* created = new ImageView(this, std::move(imageView));

	mImageViews.emplace(std::pair<uintptr_t, std::unique_ptr<ImageView>>(uintptr_t(created), created));

	return created;

}
