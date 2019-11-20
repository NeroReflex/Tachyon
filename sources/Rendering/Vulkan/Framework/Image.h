#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				/**
				 * Represents a buffer used to store an image or texture.
				 */
				class Image final :
					virtual public DeviceOwned{

				public:
					Image(const Device* device, bool swapchainImage, VkImage&& image) noexcept;

					Image(const Image&) = delete;

					Image(Image&&) = delete;

					Image& operator=(const Image&) = delete;

					~Image() final;

					const VkImage& getNativeImageHandle() const noexcept;

				private:
					VkImage mImage;

					bool mIsSwapchainImage;
				};
			}
		}
	}
}