#pragma once

#include "ImageInterface.h"
#include "ImageView.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				/**
				 * Represents a buffer used to store an image or texture.
				 */
				class Image final :
					virtual public ImageInterface {

				public:
					enum class ImageType {
						Image1D,
						Image2D,
						Image3D,
						CubeMap
					};

					Image(const Device* device, VkImage&& image) noexcept;

					Image(const Image&) = delete;

					Image(Image&&) = delete;

					Image& operator=(const Image&) = delete;

					~Image() final;

					ImageView* createImageView(ImageView::ViewType type, VkFormat format, VkImageAspectFlagBits subrangeAspectBits = VK_IMAGE_ASPECT_COLOR_BIT, ViewColorMapping swizzle = ViewColorMapping::rgba_rgba, uint32_t subrangeBaseMipLevel = 0, uint32_t subrangeLevelCount = 1, uint32_t subrangeBaseArrayLayer = 0, uint32_t subrangeLayerCount = 1) noexcept;

				private:
					std::unordered_map<uintptr_t, std::unique_ptr<ImageView>> mImageViews;
				};
			}
		}
	}
}

