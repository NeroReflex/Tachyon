#pragma once

#include "ImageInterface.h"

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
					Image(const Device* device, VkImage&& image) noexcept;

					Image(const Image&) = delete;

					Image(Image&&) = delete;

					Image& operator=(const Image&) = delete;

					~Image() final;

				};
			}
		}
	}
}

