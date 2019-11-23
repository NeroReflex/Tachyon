#pragma once

#include "ImageInterface.h"
#include "SpaceRequiringResource.h"
#include "ImageView.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				/**
				 * Represents a buffer used to store an image or texture.
				 */
				class Image final :
					public SpaceRequiringResource,
					public ImageInterface {

				public:
					enum class ImageType {
						Image1D,
						Image2D,
						Image3D
					};

					Image(const Device* device, ImageType type, VkFormat format, VkExtent3D extent, VkSampleCountFlagBits samples, uint32_t mipLevels, VkImage&& image) noexcept;

					Image(const Image&) = delete;

					Image(Image&&) = delete;

					Image& operator=(const Image&) = delete;

					~Image() final;

					ImageView* createImageView(ImageView::ViewType type, VkFormat format, VkImageAspectFlagBits subrangeAspectBits = VK_IMAGE_ASPECT_COLOR_BIT, ImageView::ViewColorMapping swizzle = ImageView::ViewColorMapping::rgba_rgba, uint32_t subrangeBaseMipLevel = 0, uint32_t subrangeLevelCount = 1, uint32_t subrangeBaseArrayLayer = 0, uint32_t subrangeLayerCount = 1) noexcept;

					const VkFormat& getFormat() const noexcept override;

				private:
					ImageType mType;

					VkExtent3D mExtent;

					VkFormat mFormat;

					uint32_t mMipLevels;

					VkSampleCountFlagBits mSamples;

					std::unordered_map<uintptr_t, std::unique_ptr<ImageView>> mImageViews;
				};
			}
		}
	}
}

