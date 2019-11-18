#pragma once

#pragma once

#include "InstanceObject.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {

				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class Surface :
					virtual public InstanceObject {

				public:
					Surface(const std::shared_ptr<Instance>& instance, VkSurfaceKHR surface) noexcept;

					Surface(const Surface&) = delete;

					Surface(Surface&&) = delete;

					Surface& operator=(const Surface&) = delete;

					~Surface() override;

					const VkSurfaceKHR& getNativeHandle() const noexcept;

				private:

					VkSurfaceKHR mSurface;

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
