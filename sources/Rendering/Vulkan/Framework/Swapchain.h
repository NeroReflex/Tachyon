#pragma once

#include "DeviceMemoryBuffer.h"
#include "SwapchainImage.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Swapchain :
					virtual public DeviceMemoryBuffer {

				public:
					Swapchain(Device* device, VkSwapchainKHR&& swapchain, VkFormat format, uint32_t width, uint32_t height) noexcept;

					Swapchain(Swapchain&) = delete;

					Swapchain& operator=(Swapchain&) = delete;

					~Swapchain() override;

					const uint32_t& getWidth() const noexcept;

					const uint32_t& getHeight() const noexcept;

					const VkFormat& getFormat() const noexcept;

				private:
					VkSwapchainKHR mSwapchain;

					VkFormat mFormat;
					
					std::vector<std::unique_ptr<SwapchainImage>> mFramebuffers;

					uint32_t mWidth;

					uint32_t mHeight;

				};

			}
		}
	}
}
