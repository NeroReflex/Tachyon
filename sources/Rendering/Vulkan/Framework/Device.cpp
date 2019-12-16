#include "Device.h"

#include "ComputePipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

VkPresentModeKHR Device::SwapchainSelector::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept {
	for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR Device::SwapchainSelector::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkSwapchainCreateInfoKHR Device::SwapchainSelector::operator()(const SwapChainSupportDetails& swapChainSupport) const noexcept {
	VkSwapchainCreateInfoKHR createInfo;

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    	imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.presentMode = presentMode;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;

	return createInfo;
}

Device::Device(const Instance* instance, VkPhysicalDevice&& physicalDevice, VkDevice&& device, std::vector<std::tuple<QueueFamily::ConcreteQueueFamilyDescriptor, uint32_t>> requiredQueueFamilyCollection) noexcept
	: InstanceOwned(instance),
	mPhysicalDevice(physicalDevice),
	mDevice(device) {
	// Query avaliable extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr);
	
	mAvailableExtensions.resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, mAvailableExtensions.data());

	for (const auto& requiredQueueFamily : requiredQueueFamilyCollection) {
		mQueueFamilies.push_back(new QueueFamily(this, std::get<0>(requiredQueueFamily), std::get<1>(requiredQueueFamily)));
	}

	// Query swapchain support
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &mSupportedSwapchain.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, getParentInstance()->getSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		mSupportedSwapchain.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, getParentInstance()->getSurface(), &formatCount, mSupportedSwapchain.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		mSupportedSwapchain.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, getParentInstance()->getSurface(), &presentModeCount, mSupportedSwapchain.presentModes.data());
	}

	DBG_ASSERT((isExtensionAvailable(VK_KHR_SWAPCHAIN_EXTENSION_NAME)));

	DBG_ASSERT(((!mSupportedSwapchain.formats.empty()) && (!mSupportedSwapchain.presentModes.empty())));
}

Device::~Device() {
	mSwapchain.reset();

	// Remove each object
	mOwnedObjects.clear();

	vkDestroyDevice(mDevice, nullptr);
}

const QueueFamily* Device::getQueueFamily(uint32_t index) const noexcept {
	DBG_ASSERT((index < (mQueueFamilies.size())));

	return mQueueFamilies[index];
}

bool Device::isExtensionAvailable(const std::string& extName) const noexcept {
	for (const auto& extension : mAvailableExtensions) {
		if (strcmp(extName.c_str(), extension.extensionName) == 0) {
			return true;
		}
	}

	return false;
}

const VkDevice& Tachyon::Rendering::Vulkan::Framework::Device::getNativeDeviceHandle() const noexcept {
	return mDevice;
}

const VkPhysicalDevice& Tachyon::Rendering::Vulkan::Framework::Device::getNativePhysicalDeviceInstance() const noexcept {
	return mPhysicalDevice;
}

const ComputeShader* Device::loadComputeShader(const ShaderLayoutBinding& bindings, const char* source, uint32_t size) noexcept {
	return new ComputeShader(this, bindings, source, size);
}

const Pipeline* Device::createPipeline(const std::vector<const Shader*>& shaders) noexcept
{
	DBG_ASSERT( (shaders.size() > 0) );
	bool isComputePipeline = (shaders.size() == 1) && (shaders[0]->getType() == Shader::ShaderType::Compute);

	std::vector<VkPipelineShaderStageCreateInfo> shadersStageInfo(shaders.size());
	std::vector<VkDescriptorSetLayoutBinding> renderDescriptorSetLayoutBinding;
	
	uint32_t i = 0;
	for (const auto& shader : shaders) {
		VkShaderStageFlagBits stageType;
		switch (shader->getType()) {
		case Shader::ShaderType::Vertex:
			stageType = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case Shader::ShaderType::Fragment:
			stageType = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case Shader::ShaderType::Geometry:
			stageType = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case Shader::ShaderType::Compute:
			stageType = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		default:
			DBG_ASSERT(false);
		}

		shadersStageInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shadersStageInfo[i].pNext = nullptr;
		shadersStageInfo[i].stage = stageType;
		shadersStageInfo[i].module = shader->getNativeShaderModuleHandle();
		shadersStageInfo[i].pName = "main";
		shadersStageInfo[i].pSpecializationInfo = nullptr;

		const auto& shaderBindings = shader->getNativeShaderBindings();
		renderDescriptorSetLayoutBinding.insert(renderDescriptorSetLayoutBinding.cend(), shaderBindings.cbegin(), shaderBindings.cend());

		++i;
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = NULL;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(renderDescriptorSetLayoutBinding.size()); // only a single binding in this descriptor set layout. 
	descriptorSetLayoutCreateInfo.pBindings = renderDescriptorSetLayoutBinding.data();

	VkDescriptorSetLayout descriptorSetLayout;
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
	
	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

	VK_CHECK_RESULT(vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

	Pipeline* result = nullptr;

	VkPipeline pipeline;
	if (isComputePipeline) {
		VkComputePipelineCreateInfo computePipelineCreateInfo;
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
		computePipelineCreateInfo.flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif
		computePipelineCreateInfo.stage = shadersStageInfo[0];
		computePipelineCreateInfo.layout = pipelineLayout;
		
		VK_CHECK_RESULT(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline));

		result = new ComputePipeline(this, std::move(pipelineLayout), std::move(descriptorSetLayout), std::move(pipeline));
	} else {
		DBG_ASSERT(false);
	}

	DBG_ASSERT(result);

	return result;
}

Swapchain* Device::createSwapchain(std::vector<const QueueFamily*> queueFamilyCollection, uint32_t width, uint32_t height, const SwapchainSelector& selector) noexcept {
	DBG_ASSERT( (!mSwapchain) );

	DBG_ASSERT((queueFamilyCollection.size() >= 1));

	VkExtent2D actualExtent = { width, height };

	std::vector<uint32_t> sharingQueueFamilyCollection;
	for (const auto& sharingQueue : queueFamilyCollection) {
		sharingQueueFamilyCollection.push_back(sharingQueue->getNativeQueueFamilyIndexHandle());
	}

	VkSwapchainCreateInfoKHR createInfo = selector(mSupportedSwapchain);
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	createInfo.surface = getParentInstance()->getSurface();
	createInfo.imageSharingMode = (queueFamilyCollection.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(sharingQueueFamilyCollection.size());
    createInfo.pQueueFamilyIndices = sharingQueueFamilyCollection.data();
	actualExtent.width = std::max(mSupportedSwapchain.capabilities.minImageExtent.width, std::min(mSupportedSwapchain.capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(mSupportedSwapchain.capabilities.minImageExtent.height, std::min(mSupportedSwapchain.capabilities.maxImageExtent.height, actualExtent.height));
	createInfo.imageExtent = actualExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR /*VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR*/;
	createInfo.clipped = VK_TRUE;
	createInfo.flags = 0;
	
	VkBool32 isSurfaceSupported = VK_FALSE;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, queueFamilyCollection[0]->getNativeQueueFamilyIndexHandle(), getParentInstance()->getSurface(), &isSurfaceSupported));
	DBG_ASSERT((isSurfaceSupported == VK_TRUE));

	VkSwapchainKHR swapchain;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &swapchain));

	mSwapchain.reset(new Swapchain(this, std::move(swapchain), createInfo.imageFormat, actualExtent.width, actualExtent.height));
	return mSwapchain.get();
}

Swapchain* Device::getSwapchain() const noexcept {
	return mSwapchain.get();
}

Image* Device::createImage(std::vector<const QueueFamily*> queueFamilyCollection, Image::ImageType type, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, uint32_t mipLevels, VkSampleCountFlagBits samples) noexcept {
	VkImageType imgType = VK_IMAGE_TYPE_3D;
	switch (type) {
	case Image::ImageType::Image1D:
		imgType = VK_IMAGE_TYPE_1D;
		height = 1;
		depth = 1;
		break;

	case Image::ImageType::Image2D:
		imgType = VK_IMAGE_TYPE_2D;
		depth = 1;
		break;

	case Image::ImageType::Image3D:
		imgType = VK_IMAGE_TYPE_3D;
		break;

	default:
		DBG_ASSERT(false);
	}

	std::vector<uint32_t> sharingQueueFamilyCollection;
	for (const auto& sharingQueue : queueFamilyCollection) {
		sharingQueueFamilyCollection.push_back(sharingQueue->getNativeQueueFamilyIndexHandle());
	}

	VkImageCreateInfo imageCreateInfo; // Using stack will lead to stack overflow
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	imageCreateInfo.sharingMode = (sharingQueueFamilyCollection.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(sharingQueueFamilyCollection.size());
	imageCreateInfo.pQueueFamilyIndices = sharingQueueFamilyCollection.data();
	imageCreateInfo.format = format;
	imageCreateInfo.samples = samples;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.imageType = imgType;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;

	// This is to create the ModelMatrix-Collection
	VkImage image;
	VK_CHECK_RESULT(vkCreateImage(mDevice, &imageCreateInfo, nullptr, &image));

	return new Image(this, type, format, imageCreateInfo.extent, samples, mipLevels, std::move(image));
}

void Device::allocateResources(VkMemoryPropertyFlagBits props, const std::initializer_list<SpaceRequiringResource*>& resources) noexcept {
	DBG_ASSERT((resources.size() > 0));
	uint32_t memoryTypeBits = 0xFFFFFFFF;
	VkDeviceSize minimumBlockCount = 0;
	for (const auto& allocRequiringResource : resources) {
		const auto requiredMemory = allocRequiringResource->getMemoryRequirements();
		minimumBlockCount += MemoryPool::getAtomicMemoryBlockCount(static_cast<VkDeviceSize>(requiredMemory.size), static_cast<VkDeviceSize>(requiredMemory.alignment), minimumBlockCount);
		memoryTypeBits &= requiredMemory.memoryTypeBits;
	}

	DBG_ASSERT((memoryTypeBits != 0));

	MemoryPool* createdPool = registerMemoryPool(props, minimumBlockCount + 32, memoryTypeBits);

	VkDeviceSize lastAlloc = 0;
	for (auto& allocRequiringResource : resources) {
		lastAlloc = createdPool->malloc(*allocRequiringResource, lastAlloc);
		allocRequiringResource->execBinding(this, createdPool, lastAlloc);
	}
	
}

MemoryPool* Device::registerMemoryPool(VkMemoryPropertyFlagBits props, VkDeviceSize blockCount, uint32_t memoryTypeBits) noexcept {
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = blockCount * Memory::atomicMemoryBlockSize;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryTypeBits, props);

	// Allocate a big chunk of memory on the device
	VkDeviceMemory memoryPool;
	VK_CHECK_RESULT(vkAllocateMemory(mDevice, &allocateInfo, NULL, &memoryPool));

	MemoryPool* createdPool = new MemoryPool(this, props, memoryTypeBits, blockCount, std::move(memoryPool));

	mMemoryPools[props].push_back(createdPool);

	return createdPool;
}

CommandPool* Device::createCommandPool(const QueueFamily* queueFamily) noexcept {
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    // the queue family of this command pool. All command buffers allocated from this command pool,
	// must be submitted to queues of this family ONLY. 
	commandPoolCreateInfo.queueFamilyIndex = queueFamily->getNativeQueueFamilyIndexHandle();
	
	VkCommandPool cmdPool;
	VK_CHECK_RESULT(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &cmdPool));
	
	return new CommandPool(this, std::move(cmdPool));
}

uint32_t Device::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const noexcept {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memoryProperties);

	/*
	How does this search work?
	See the documentation of VkPhysicalDeviceMemoryProperties for a detailed description.
	*/
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((memoryTypeBits & (1 << i)) &&
			((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}
	return -1;
}

DescriptorPool* Device::createDescriptorPool(const std::vector<std::tuple<ShaderLayoutBinding::BindingType, uint32_t>>& descriptorPoolSize, uint32_t maxSets) noexcept {
	DBG_ASSERT((maxSets > 0));
	
	std::vector<VkDescriptorPoolSize> descriptorPoolSizeNativeCollection(descriptorPoolSize.size());
	for (uint32_t k = 0; k < descriptorPoolSize.size(); ++k) {
		VkDescriptorType nativeType;
		switch (std::get<0>(descriptorPoolSize[k])) {
		case ShaderLayoutBinding::BindingType::Sampler:
			nativeType = VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		case ShaderLayoutBinding::BindingType::CombinedImageSampler:
			nativeType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		case ShaderLayoutBinding::BindingType::SampledImage:
			nativeType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			break;
		case ShaderLayoutBinding::BindingType::StorageImage:
			nativeType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			break;
		case ShaderLayoutBinding::BindingType::UniformTexelBuffer:
			nativeType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			break;
		case ShaderLayoutBinding::BindingType::UniformTexelStorage:
			nativeType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			break;
		case ShaderLayoutBinding::BindingType::StorageBuffer:
			nativeType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case ShaderLayoutBinding::BindingType::UniformBuffer:
			nativeType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		default:
			DBG_ASSERT(false);
		}

		descriptorPoolSizeNativeCollection[k].type = nativeType;

		DBG_ASSERT((std::get<1>(descriptorPoolSize[k]) > 0));
		descriptorPoolSizeNativeCollection[k].descriptorCount = std::get<1>(descriptorPoolSize[k]);
	}

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.maxSets = maxSets;
	createInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizeNativeCollection.size());
	createInfo.pPoolSizes = descriptorPoolSizeNativeCollection.data();
	createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	VkDescriptorPool descriptorPool;
	VK_CHECK_RESULT(vkCreateDescriptorPool(mDevice, &createInfo, nullptr, &descriptorPool));

	return new DescriptorPool(this, std::move(descriptorPool));
}

Fence* Device::createFence(bool signaled) noexcept {
	VkFence fence;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = (signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);
	VK_CHECK_RESULT(vkCreateFence(getNativeDeviceHandle(), &fenceCreateInfo, NULL, &fence));

	return new Fence(this, std::move(fence));
}

Buffer* Device::createBuffer(std::vector<const QueueFamily*> queueFamilyCollection, uint32_t size) noexcept {
	std::vector<uint32_t> sharingQueueFamilyCollection;
	for (const auto& sharingQueue : queueFamilyCollection) {
		sharingQueueFamilyCollection.push_back(sharingQueue->getNativeQueueFamilyIndexHandle());
	}

	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.sharingMode = (sharingQueueFamilyCollection.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = static_cast<uint32_t>(sharingQueueFamilyCollection.size());
	createInfo.pQueueFamilyIndices = sharingQueueFamilyCollection.data();
	createInfo.size = size;
	DBG_ASSERT(false); // TODO: createInfo.usage

	VkBuffer buffer;
	vkCreateBuffer(getNativeDeviceHandle(), &createInfo, nullptr, &buffer);

	return new Buffer(this, std::move(buffer));
}
