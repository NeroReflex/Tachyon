#pragma once

#include "InstanceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Swapchain;

				class Device :
					virtual public InstanceOwned {

				public:
					struct SwapChainSupportDetails {
						VkSurfaceCapabilitiesKHR capabilities;
						std::vector<VkSurfaceFormatKHR> formats;
						std::vector<VkPresentModeKHR> presentModes;
					};

					class SwapchainSelector {
					public:
						virtual VkSwapchainCreateInfoKHR operator()(const SwapChainSupportDetails&) const noexcept;
					
					private:
						VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept;
					};

					Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device) noexcept;

					Device(Device&) = delete;

					Device& operator=(Device&) = delete;

					~Device() override;

					Swapchain* createSwapchain(const SwapchainSelector& selector = Device::SwapchainSelector()) noexcept;

					bool isExtensionAvailable(const std::string& extName) const noexcept;

				private:
					VkPhysicalDevice mPhysicalDevice;

					VkDevice mDevice;

					std::vector<VkExtensionProperties> mAvailableExtensions;

					SwapChainSupportDetails mSupportedSwapchain;

					std::unique_ptr<Swapchain> mSwapchain;

					//std::vector<Queue> mQueueCollection;
				};

			}
		}
	}
}
