#pragma once

#include "InstanceObject.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {

				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class PhysicalDevice :
					virtual public InstanceObject {

				public:
					PhysicalDevice(const std::weak_ptr<Instance>& instance) noexcept;

					PhysicalDevice(const PhysicalDevice&) = delete;

					PhysicalDevice(PhysicalDevice&&) = delete;

					PhysicalDevice& operator=(const PhysicalDevice&) = delete;

					virtual ~PhysicalDevice();
					
				private:

				/**
				 * The physical device is some device on the system that supports usage of Vulkan.
				 * Often, it is simply a graphics card that supports Vulkan.
				 */
					VkPhysicalDevice mPhysicalDevice;

				};

			}
		}
	}
}