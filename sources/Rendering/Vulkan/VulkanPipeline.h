#pragma once

#include "Rendering/RenderingPipeline.h"

// befause a float is 4 bytes, and a vec4 is four floats
#define VULKAN_SIZEOF_RGBA32F 4 * 4

namespace Tachyon {
    namespace Rendering {
		namespace Vulkan {
			class VulkanPipeline final :
				virtual public Rendering::RenderingPipeline {
				
				struct SwapChainSupportDetails {
					VkSurfaceCapabilitiesKHR capabilities;
					std::vector<VkSurfaceFormatKHR> formats;
					std::vector<VkPresentModeKHR> presentModes;
				};

			public:
				VulkanPipeline(const VulkanPipeline&) = delete;

				VulkanPipeline(VulkanPipeline&&) = delete;

				VulkanPipeline& operator=(const VulkanPipeline&) = delete;

				~VulkanPipeline() override;

				VulkanPipeline(GLFWwindow* window) noexcept;

				void enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept final;
				
			protected:
				void onReset() noexcept final;

				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept final;

				void onRender(const Core::Camera& camera) noexcept final;

			private:
				VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept;

				SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const noexcept;

				VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept;

				VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept;

				/**
				 * Create a Vulkan instance.
				 */
				void createInstance() noexcept;

				/**
				 * Check if the given physical device support all required extensions.
				 *
				 * @param device the physical device
				 * @return TRUE iif the given device does support all required extensions
				 */
				bool checkDeviceExtensionSupport(VkPhysicalDevice device) noexcept;

				/*
				 * Select the most suitable physical device that can be used with Vulkan.
				 *
				 * Note: must be called after createInstance()
				 */
				void findPhysicalDevice() noexcept;

				/**
				 * Create the best swapchain possible for the selected device.
				 *
				 * Note: must be called after findPhysicalDevice()
				 */
				void createSwapChain() noexcept;

				/**
				 * Search for a family queue in the physical device that satisfy all requrements.
				 *
				 * Note: must be called after findPhysicalDevice()
				 */
				void pickQueueFamilyIndex() noexcept;

				/**
				 * Create the Vulkan logical device from the physical device.
				 *
				 * Note: must be called after findPhysicalDevice()
				 */
				void createLogicalDevice() noexcept;

				/**
				 * Creates all buffer required for accelection structures,
				 * geometry and other data related to the scene representation.
				 *
				 * Note: must be called after createLogicalDevice()
				 */
				void createCoreBuffers() noexcept;

				/**
				 * Destroy all buffer for acceleration scrutures created by createCoreBuffers()
				 * 
				 * Note: must be called after createCoreBuffers()
				 */
				void destroyCoreBuffers() noexcept;

				/**
				 * Find memory type with the given properties.
				 *
				 * Note: must be called after createLogicalDevice()
				 */
				uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) noexcept;

				/**
				 * Create a VkBuffer and VkDeviceMemory with the given size, memory usage bits and properties.
				 * 
				 * @param buffer the buffer that is going to be created
				 * @param bufferMemory the allocated memory that backs the buffer
				 * @param memorySize the size, in bytes to allocate
				 * @param memoryUsage buffer usage, one of VK_IMAGE_USAGE_STORAGE_BIT, ... and so on...
				 * @param memoryProperties properties of memory (a valid combination of VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				 */
				void allocateBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize memorySize, VkBufferUsageFlags memoryUsage, VkMemoryPropertyFlags memoryProperties) noexcept;

				void destroyBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory) noexcept;

				void createPipeline() noexcept;

				void destroyPipeline() noexcept;

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

				struct SurfaceSwapchain {
					VkSurfaceFormatKHR surfaceFormat;
					VkPresentModeKHR presentMode;
					VkExtent2D extent;
				} mSurfaceSwapchain;

				/**
				 * In order to use Vulkan, you must create an instance.
				 */
				VkInstance mInstance;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
				VkDebugReportCallbackEXT debugReportCallback;
#endif

				SwapChainSupportDetails mSwapChainSupport;

				/**
				 * The render surface.
				 */
				VkSurfaceKHR mSurface;

				/**
				 * The swapchain.
				 */
				VkSwapchainKHR mSwapChain;

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

				VkQueueFamilyProperties mQueueProperties;



				/**
				 * Groups of queues that have the same capabilities(for instance, they all supports graphics and computer operations),
				 * are grouped into queue families.
				 * 
				 * When submitting a command buffer, you must specify to which queue in the family you are submitting to.
				 * This variable keeps track of the index of that queue in its family.
				 */
				uint32_t mQueueFamilyIndex;

				/**
				 * This is the device memory that backs all raytracing Acceleration structures and geometry.
				 */
				VkDeviceMemory mRaytracingDeviceMemory;

				/**
				 * This is the image on the GPU used to store texels used as the scene TLAS.
				 *
				 * Note: this buffer will always be used as rgba32f
				 */
				VkImage mRaytracingTLAS;

				/**
				 * This is the image on the GPU used to store texels used as the scene BLAS collection.
				 *
				 * Note: this buffer will always be used as rgba32f
				 */
				VkImage mRaytracingBLASCollection;

				/**
				 * This is the image on the GPU used to store texels used to place an object into the 3D space.
				 *
				 * Note: this buffer will always be used as rgba32f
				 */
				VkImage mRaytracingModelMatrix;

				/**
				 * This is the image on the GPU used to store texels used tostore geometry.
				 *
				 * Note: this buffer will always be used as rgba32f
				 */
				VkImage mRaytracingGeometryCollection;

				VkPipelineLayout mRaytracerFlushPipelineLayout;
				VkPipeline mRaytracerFlushPipeline;

				VkPipelineLayout mRaytracerInsertPipelineLayout;
				VkPipeline mRaytracerInsertPipeline;

				VkShaderModule mRaytracerUpdateModule;
				VkPipeline mRaytracerUpdatePipeline;

				VkPipelineLayout mRaytracerRenderPipelineLayout;
				VkPipeline mRaytracerRenderPipeline;

				VkPipelineLayout mDisplayWriterPipelineLayout;
				VkPipeline mDisplayWriterPipeline;
			};
		}
    }
}
