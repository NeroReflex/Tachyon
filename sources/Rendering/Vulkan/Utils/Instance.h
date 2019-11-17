#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {

				class PhysicalDevice;

				/**
				 * Represents a Vulkan instance.
				 */
				class Instance {

					std::vector<const char*> mEnabledExtensions;

					std::vector<const char*> mEnabledLayers;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
					VkDebugReportCallbackEXT debugReportCallback;
#endif

					VkInstance mInstance;

					std::shared_ptr<PhysicalDevice> selectPhysicalDevice() const noexcept;

				public:
					Instance(VkInstance instance) noexcept;

					Instance(PFN_vkDebugReportCallbackEXT debugCallback, const std::vector<std::string>& requredExtensions = std::vector<std::string>()) noexcept;

					Instance(const Instance&) = delete;

					Instance(Instance&&) = delete;

					Instance& operator=(const Instance&) = delete;

					~Instance();

					const VkInstance& getNativeHandle() const noexcept;
				};
			}
		}
	}
}