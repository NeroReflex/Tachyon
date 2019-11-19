#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class InstanceOwned;
				class Device;

				class Instance {

				public:

					static const float defaultQueuePriority;

					Instance(GLFWwindow* window) noexcept;

					Instance(Instance&) = delete;

					Instance& operator=(Instance&) = delete;

					~Instance();

					Device* openDevice() noexcept;

					const VkSurfaceKHR& getSurface() const noexcept;

					static std::vector<VkExtensionProperties> getAllSupportedExtensions() noexcept;
					
					const VkInstance& getNativeInstanceHandle() const noexcept;

				private:
					void enqueueInstanceExtension(const char* extName) noexcept;

					void enqueueDeviceExtension(const char* extName) noexcept;
					
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
					std::vector<const char*> mValidationLayers;
#endif
					VkInstance mInstance;

					VkSurfaceKHR mSurface;

					char** mInstanceExtensions;

					uint32_t mInstanceExtensionsCount;

					char** mDeviceExtensions;

					uint32_t mDeviceExtensionsCount;

					std::vector<std::unique_ptr<InstanceOwned>> mObjectsCollection;
				};

			}
		}
	}
}
