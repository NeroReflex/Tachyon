#pragma once

#include "ImageInterface.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				/**
				 * Represents an image on the swapchain.
				 */
				class SwapchainImage final :
					virtual public ImageInterface {

				public:
					SwapchainImage(const Device* device, VkImage&& image) noexcept;

					SwapchainImage(const SwapchainImage&) = delete;

					SwapchainImage(SwapchainImage&&) = delete;

					SwapchainImage& operator=(const SwapchainImage&) = delete;

					~SwapchainImage() final;

				};
			}
		}
	}
}
