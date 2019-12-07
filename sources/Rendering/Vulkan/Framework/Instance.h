#pragma once

#include "QueueFamily.h"

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

					Device* openDevice(std::vector<std::vector<QueueFamily::QueueFamilySupportedOperationType>> queueDescriptors) noexcept;

					const VkSurfaceKHR& getSurface() const noexcept;

					static std::vector<VkExtensionProperties> getAllSupportedExtensions() noexcept;
					
					const VkInstance& getNativeInstanceHandle() const noexcept;

				private:
					bool corresponds(const std::vector<Tachyon::Rendering::Vulkan::Framework::QueueFamily::QueueFamilySupportedOperationType>& operations, VkQueueFamilyProperties queueFamily, VkPhysicalDevice device, uint32_t familyIndex) const noexcept;
					
					void enqueueInstanceExtension(const char* extName) noexcept;

					void enqueueDeviceExtension(const char* extName) noexcept;
					
					// TODO: implement the usage of this thing!
					VkAllocationCallbacks mCustomAllocator;

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
