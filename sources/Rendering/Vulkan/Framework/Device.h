#pragma once

#include "InstanceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Device :
					virtual public InstanceOwned {

				public:
					Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device) noexcept;

					Device(Device&) = delete;

					Device& operator=(Device&) = delete;

					~Device() override;

					bool isExtensionAvailable(const std::string& extName) const noexcept;

				private:
					VkPhysicalDevice mPhysicalDevice;

					VkDevice mDevice;

					std::vector<VkExtensionProperties> mAvailableExtensions;

					struct SwapChainSupportDetails {
						VkSurfaceCapabilitiesKHR capabilities;
						std::vector<VkSurfaceFormatKHR> formats;
						std::vector<VkPresentModeKHR> presentModes;
					} mSupportedSwapchain;

					//std::vector<Queue> mQueueCollection;
				};

			}
		}
	}
}
