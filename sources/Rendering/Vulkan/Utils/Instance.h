#pragma once

#include "VulkanManaged.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {
				class InstanceObject;
				class PhysicalDevice;

				/**
				 * Represents a Vulkan instance.
				 */
				class Instance :
					virtual public VulkanManaged {

					friend class InstanceObject;

					std::vector<const char*> mEnabledExtensions;

					std::vector<const char*> mEnabledLayers;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
					VkDebugReportCallbackEXT debugReportCallback;
#endif

					VkInstance mInstance;

					

				public:
					class PhysicalDeviceSelector {
					public:
						virtual bool operator()(VkPhysicalDevice physicalDevice) const noexcept = 0;
					};

					class PhysicalDeviceSupportingExtensionsSelector :
						virtual public PhysicalDeviceSelector {

						std::vector<const char*> mRequiredExtensions;

					public:
						PhysicalDeviceSupportingExtensionsSelector(std::vector<const char*> extensions) noexcept;

						/**
						 * Check if the given physical device support all required extensions.
						 *
						 * @param device the physical device
						 * @return TRUE iif the given device does support all required extensions
						 */
						bool operator()(VkPhysicalDevice physicalDevice) const noexcept override;
					};

					Instance(VkInstance instance) noexcept;

					Instance(PFN_vkDebugReportCallbackEXT debugCallback, const std::vector<std::string>& requiredExtensions = std::vector<std::string>()) noexcept;

					Instance(const Instance&) = delete;

					Instance(Instance&&) = delete;

					Instance& operator=(const Instance&) = delete;

					~Instance();

					const VkInstance& getNativeHandle() const noexcept;

					std::shared_ptr<PhysicalDevice> selectPhysicalDevice(const PhysicalDeviceSelector& selector, std::optional<VkSurfaceKHR> surface = std::optional<VkSurfaceKHR>()) const noexcept;
				};
			}
		}
	}
}