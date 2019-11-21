#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				/**
				 * Represents a buffer used to store an image or texture.
				 */
				class ImageInterface final :
					virtual public DeviceOwned {

				public:
					inline ImageInterface(const Device* device, VkImage&& image) noexcept : DeviceOwned(device), mImage(std::move(image)) {};

					ImageInterface(const ImageInterface&) = delete;

					ImageInterface(ImageInterface&&) = delete;

					ImageInterface& operator=(const ImageInterface&) = delete;

					~ImageInterface() override = default;

					inline const VkImage& getNativeImageHandle() const noexcept {
                        return mImage;
                    };

				private:
					VkImage mImage;
				};
			}
		}
	}
}