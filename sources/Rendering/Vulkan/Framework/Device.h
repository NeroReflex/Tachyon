#pragma once

#include "InstanceOwned.h"
#include "Swapchain.h"
#include "ShaderLayoutBinding.h"
#include "Pipeline.h"
#include "Image.h"
#include "CommandPool.h"
#include "MemoryPool.h"
#include "DescriptorPool.h"

#include "ComputeShader.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Device :
					virtual public InstanceOwned {

				public:
					struct SwapChainSupportDetails {
						VkSurfaceCapabilitiesKHR capabilities;
						std::vector<VkSurfaceFormatKHR> formats;
						std::vector<VkPresentModeKHR> presentModes;
					};

					class SwapchainSelector {
					public:
						virtual VkSwapchainCreateInfoKHR operator()(const SwapChainSupportDetails&) const noexcept;
					
					private:
						VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept;
						VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept;
					};

					Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device, uint32_t queueFamilyIndex) noexcept;

					Device(Device&) = delete;

					Device& operator=(Device&) = delete;

					~Device() override;

					Swapchain* createSwapchain(uint32_t width, uint32_t height, const SwapchainSelector& selector = Device::SwapchainSelector()) noexcept;

					Swapchain* getSwapchain() const noexcept;

					CommandPool* createCommandPool() noexcept;
					
					bool isExtensionAvailable(const std::string& extName) const noexcept;

					const VkDevice& getNativeDeviceHandle() const noexcept;

					const VkPhysicalDevice& getNativePhysicalDeviceInstance() const noexcept;

					const ComputeShader* loadComputeShader(const ShaderLayoutBinding& bindings, const char* source, uint32_t size) noexcept;

					DescriptorPool* createDescriptorPool(const std::vector<std::tuple<ShaderLayoutBinding::BindingType, uint32_t>>& descriptorPoolSize, uint32_t maxSets) noexcept;

					const Pipeline* createPipeline(const std::vector<const Shader*>& shaders) noexcept;

					Image* createImage(Image::ImageType type, uint32_t width, uint32_t height = 1, uint32_t depth = 1, VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT, uint32_t mipLevels = 1, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) noexcept;

					void allocateResources(VkMemoryPropertyFlagBits props, const std::initializer_list<SpaceRequiringResource*>& resources) noexcept;

				private:
					template <typename T>
					T* registerNewOwnedObj(T* const ownedObj) noexcept {
						uintptr_t ptr = uintptr_t(ownedObj);

						mOwnedObjects.emplace(std::pair<uintptr_t, std::unique_ptr<DeviceOwned>>(ptr, ownedObj));

						return ownedObj;
					}

					uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const noexcept;

					VkPhysicalDevice mPhysicalDevice;

					VkDevice mDevice;

					uint32_t mQueueFamilyIndex;

					std::vector<VkExtensionProperties> mAvailableExtensions;

					SwapChainSupportDetails mSupportedSwapchain;

					std::unique_ptr<Swapchain> mSwapchain;

					//std::vector<Queue> mQueueCollection;

					std::unordered_map<VkMemoryPropertyFlagBits, std::list<std::unique_ptr<MemoryPool>>> mMemoryPools;

					std::unordered_map<uintptr_t, std::unique_ptr<DeviceOwned>> mOwnedObjects;

					MemoryPool* registerMemoryPool(VkMemoryPropertyFlagBits props, VkDeviceSize pagesCount, uint32_t memoryTypeBits) noexcept;

				};

			}
		}
	}
}
