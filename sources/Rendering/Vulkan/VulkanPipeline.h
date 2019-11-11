#pragma once

#include "Rendering/RenderingPipeline.h"

namespace Tachyon {
    namespace Rendering {
		namespace Vulkan {
			class VulkanPipeline final :
				virtual public Rendering::RenderingPipeline {
			public:
				VulkanPipeline(const VulkanPipeline&) = delete;

				VulkanPipeline(VulkanPipeline&&) = delete;

				VulkanPipeline& operator=(const VulkanPipeline&) = delete;

				~VulkanPipeline() override;

				VulkanPipeline() noexcept;

				void enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept final;
				
			protected:
				void onReset() noexcept final;

				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept final;

				void onRender(const Core::Camera& camera) noexcept final;

			private:
				/**
				 * Create a Vulkan instance.
				 */
				void createInstance() noexcept;

				/*
				 * Select the most suitable physical device that can be used with Vulkan.
				 *
				 * Note: must be called after createInstance()
				 */
				void findPhysicalDevice() noexcept;

				/**
				 * 
				 *
				 * Note: must be called after findPhysicalDevice()
				 *
				 */
				glm::uint32 VulkanPipeline::getQueueFamilyIndex() noexcept;

				/**
				 * Create the Vulkan logical device from the physical device.
				 *
				 * Note: must be called after findPhysicalDevice()
				 */
				void createLogicalDevice() noexcept;

				/**
				 * Creates all buffer required for accelection structures, geometry and other data related to the scene representation.
				 *
				 * Note: must be called after createLogicalDevice()
				 */
				void createCoreBuffers() noexcept;

				static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
					VkDebugReportFlagsEXT                       flags,
					VkDebugReportObjectTypeEXT                  objectType,
					uint64_t                                    object,
					size_t                                      location,
					int32_t                                     messageCode,
					const char* pLayerPrefix,
					const char* pMessage,
					void* pUserData);

				std::vector<const char*> mEnabledExtensions;

				std::vector<const char*> mEnabledLayers;

				/**
				 * In order to use Vulkan, you must create an instance.
				 */
				VkInstance mInstance;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
				VkDebugReportCallbackEXT debugReportCallback;
#endif

				/**
				 * The physical device is some device on the system that supports usage of Vulkan.
				 * Often, it is simply a graphics card that supports Vulkan.
				 */
				VkPhysicalDevice mPhysicalDevice;
				
				/**
				 * Then we have the logical device VkDevice, which basically allows
				 * us to interact with the physical device.
				 */
				VkDevice mDevice;

				/**
				 * In order to execute commands on a device(GPU), the commands must be submitted
				 * to a queue. The commands are stored in a command buffer, and this command buffer
				 * is given to the queue.
				 * There will be different kinds of queues on the device. Not all queues support
				 * graphics operations, for instance.
				 */
				VkQueue mQueue;

				/**
				 * Groups of queues that have the same capabilities(for instance, they all supports graphics and computer operations),
				 * are grouped into queue families.
				 * 
				 * When submitting a command buffer, you must specify to which queue in the family you are submitting to.
				 * This variable keeps track of the index of that queue in its family.
				 */
				uint32_t mQueueFamilyIndex;
			};
		}
    }
}
