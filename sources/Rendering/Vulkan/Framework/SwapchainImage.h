#pragma once

#include "ImageInterface.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Swapchain;

				/**
				 * Represents an image on the swapchain.
				 */
				class SwapchainImage final :
					virtual public ImageInterface {

				public:
					SwapchainImage(const Device* device, const Swapchain* swapchain, VkImage&& image) noexcept;

					SwapchainImage(const SwapchainImage&) = delete;

					SwapchainImage(SwapchainImage&&) = delete;

					SwapchainImage& operator=(const SwapchainImage&) = delete;

					~SwapchainImage() final;

					const Swapchain* getParentSwapchain() const noexcept;

					ImageView* getImageView() const noexcept;

				private:
					const Swapchain* mParentSwapchain;

					std::unique_ptr<ImageView> mImageView;
				};
			}
		}
	}
}
