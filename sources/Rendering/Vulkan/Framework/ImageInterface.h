#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class ImageView;

				/**
				 * Represents a buffer used to store an image or texture.
				 */
				class ImageInterface :
					virtual public DeviceOwned {

				public:
					
					enum class ViewType {
						Image1D,
						Image2D,
						Image3D,
						CubeMap,
						Image1DArray,
						Image2DArray,
						CubeMapArray,
					};

					enum class ViewColorMapping {
						rgba_rgba,
					};

					
					ImageInterface(const Device* device, VkImage&& image) noexcept;

					ImageInterface(const ImageInterface&) = delete;

					ImageInterface(ImageInterface&&) = delete;

					ImageInterface& operator=(const ImageInterface&) = delete;

					~ImageInterface() override;

					const VkImage& getNativeImageHandle() const noexcept;

					ImageView* createImageView(ViewType type, VkFormat format, VkImageAspectFlagBits subrangeAspectBits = VK_IMAGE_ASPECT_COLOR_BIT, ViewColorMapping swizzle = ViewColorMapping::rgba_rgba, uint32_t subrangeBaseMipLevel = 0, uint32_t subrangeLevelCount = 1, uint32_t subrangeBaseArrayLayer = 0, uint32_t subrangeLayerCount = 1) noexcept;

				private:
					VkImage mImage;

					std::unordered_map<uintptr_t, std::unique_ptr<ImageView>> mImageViews;
				};
			}
		}
	}
}
