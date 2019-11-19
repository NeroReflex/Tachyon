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
						VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept;
						VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept;
					};

					Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device, uint32_t queueFamilyIndex) noexcept;

					Device(Device&) = delete;

					Device& operator=(Device&) = delete;

					~Device() override;

					Swapchain* createSwapchain(uint32_t width, uint32_t height, const SwapchainSelector& selector = Device::SwapchainSelector()) noexcept;

					Swapchain* getSwapchain() const noexcept;

					bool isExtensionAvailable(const std::string& extName) const noexcept;

					const VkDevice& getNativeDeviceHandle() const noexcept;

				private:
					VkPhysicalDevice mPhysicalDevice;

					VkDevice mDevice;

					uint32_t mQueueFamilyIndex;

					std::vector<VkExtensionProperties> mAvailableExtensions;

					SwapChainSupportDetails mSupportedSwapchain;

					std::unique_ptr<Swapchain> mSwapchain;

					//std::vector<Queue> mQueueCollection;
				};

			}
		}
	}
}
