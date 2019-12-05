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
					ImageInterface(Device* device, VkImage&& image) noexcept;

					ImageInterface(const ImageInterface&) = delete;

					ImageInterface(ImageInterface&&) = delete;

					ImageInterface& operator=(const ImageInterface&) = delete;

					~ImageInterface() override;

					const VkImage& getNativeImageHandle() const noexcept;

					virtual const VkFormat& getFormat() const noexcept = 0;

				private:
					VkImage mImage;
				};
			}
		}
	}
}
