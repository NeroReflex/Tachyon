#pragma once

#include "InstanceObject.h"
#include "PhysicalDevice.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {

				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class Device :
					virtual public InstanceObject {

				public:
					Device(const std::shared_ptr<Instance>& instance, VkDevice&& device, VkQueue&& initialQueue) noexcept;

					Device(const Device&) = delete;

					Device(Device&&) = delete;

					Device& operator=(const Device&) = delete;

					~Device() override;

					const VkDevice& getNativeHandle() const noexcept;

				private:

					const VkDevice mDevice;

					std::vector<VkQueue> mQueueCollection;

					/*
					VkSurfaceCapabilitiesKHR mPhysicalDeviceSwapchainCapabilities;

					std::vector<VkSurfaceFormatKHR> mPhysicalDeviceSwapchainFormats;

					std::vector<VkPresentModeKHR> mPhysicalDeviceSwapchainPresentModes;
					*/
				};

			}
		}
	}
}
