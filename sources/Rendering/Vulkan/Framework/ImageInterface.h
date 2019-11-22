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
					enum class ViewColorMapping {
						rgba_rgba,
					};

					ImageInterface(const Device* device, VkImage&& image) noexcept;

					ImageInterface(const ImageInterface&) = delete;

					ImageInterface(ImageInterface&&) = delete;

					ImageInterface& operator=(const ImageInterface&) = delete;

					~ImageInterface() override;

					const VkImage& getNativeImageHandle() const noexcept;

				private:
					VkImage mImage;
				};
			}
		}
	}
}
