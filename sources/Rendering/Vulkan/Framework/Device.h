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

					~Device();

				private:
					VkPhysicalDevice mPhysicalDevice;

					VkDevice mDevice;

					//std::vector<Queue> mQueueCollection;
				};

			}
		}
	}
}
