#pragma once

#include "ImageInterface.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class ImageView {

				public:
					
					ImageView(const ImageInterface* image, VkImageView&& imageView) noexcept;

					ImageView(const ImageView&) = delete;

					ImageView(ImageView&&) = delete;

					ImageView& operator=(const ImageView&) = delete;

					virtual ~ImageView();

					const ImageInterface* getParentImage() const noexcept;

					const VkImageView& getNativeImageViewHandle() const noexcept;

				private:
					const ImageInterface* mParentImage;

					VkImageView mImageView;
				};
			}
		}
	}
}
