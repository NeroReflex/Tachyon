#pragma once

#include "InstanceObject.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {
				class Surface;
				class Device;

				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class PhysicalDevice :
					virtual public InstanceObject {

				public:

					class QueueFamilySelector {
					public:
						virtual bool operator()(VkInstance instance, VkPhysicalDevice physicalDevice, const uint32_t& queueFamilyIndex, const VkQueueFamilyProperties& queueProperties) const noexcept = 0;
					};

					PhysicalDevice(const std::shared_ptr<Instance>& instance, VkPhysicalDevice physicalDevice, std::optional<VkSurfaceKHR> surface) noexcept;

					PhysicalDevice(const PhysicalDevice&) = delete;

					PhysicalDevice(PhysicalDevice&&) = delete;

					PhysicalDevice& operator=(const PhysicalDevice&) = delete;

					~PhysicalDevice() override;

					const VkPhysicalDevice& getNativeHandle() const noexcept;

					std::shared_ptr<Device> selectQueueFamily(const QueueFamilySelector& selector) const noexcept;
					
				private:
					
					std::shared_ptr<Surface> mSurface;

					VkSurfaceCapabilitiesKHR mPhysicalDeviceSwapchainCapabilities;
					
					std::vector<VkSurfaceFormatKHR> mPhysicalDeviceSwapchainFormats;
					
					std::vector<VkPresentModeKHR> mPhysicalDeviceSwapchainPresentModes;

					/**
					 * The physical device is some device on the system that supports usage of Vulkan.
					 * Often, it is simply a graphics card that supports Vulkan.
					 */
					VkPhysicalDevice mPhysicalDevice;

					VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

					VkPhysicalDeviceProperties mPhysicalDeviceProperties;
				};

			}
		}
	}
}
