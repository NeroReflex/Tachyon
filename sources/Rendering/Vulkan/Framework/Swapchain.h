#pragma once

#include "DeviceMemoryBuffer.h"
#include "Image.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Swapchain :
					virtual public DeviceMemoryBuffer {

				public:
					Swapchain(const Device* device, VkSwapchainKHR&& swapchain, uint32_t width, uint32_t height) noexcept;

					Swapchain(Swapchain&) = delete;

					Swapchain& operator=(Swapchain&) = delete;

					~Swapchain() override;

				private:
					VkSwapchainKHR mSwapchain;
					
					std::vector<std::unique_ptr<Image>> mFramebuffers;

					uint32_t mWidth;

					uint32_t mHeight;

				};

			}
		}
	}
}
