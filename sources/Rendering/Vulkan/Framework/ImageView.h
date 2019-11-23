#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class ImageInterface;

				class ImageView {

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

