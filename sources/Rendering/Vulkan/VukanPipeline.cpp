#include "VulkanPipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanPipeline::debugReportCallbackFn(
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData) {

	printf("Debug Report: %s: %s\n", pLayerPrefix, pMessage);

	return VK_FALSE;
}

VulkanPipeline::VulkanPipeline() noexcept {
	createInstance();
	findPhysicalDevice();
	createLogicalDevice();
	createCoreBuffers();
}

VulkanPipeline::~VulkanPipeline() {
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
		// destroy callback.
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT");

		DBG_ASSERT(func != nullptr); // Could not load vkDestroyDebugReportCallbackEXT

		func(mInstance, debugReportCallback, NULL);
#endif

	// The first step is to destroy all memory buffers and allocated memory on the GPU
	destroyCoreBuffers();

	/*
	vkDestroyShaderModule(device, computeShaderModule, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyCommandPool(mDevice, commandPool, NULL);*/

	// at this point everything about the logical device has been destroyed, so it's safe to destroy the device
	vkDestroyDevice(mDevice, NULL);

	// As I am for sure not going to use vulkan for anything else at this point just destroy the instance
	vkDestroyInstance(mInstance, NULL);
}

void VulkanPipeline::findPhysicalDevice() noexcept {
	//list all physical devices on the system with vkEnumeratePhysicalDevices
	glm::uint32 deviceCount;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, NULL);
	
	DBG_ASSERT( (deviceCount > 0) ); // could not find a device with Vulkan support

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	// TODO: better check features
	bool deviceFound = false;
	for (VkPhysicalDevice physicalDevice : devices) {

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);


		if ((deviceProperties.limits.maxComputeWorkGroupInvocations >= (32*48)) &&
			(deviceProperties.limits.maxImageDimension1D >= mRaytracerRequirements.mTLASTexels_Width) &&
			(deviceProperties.limits.maxImageDimension2D >= glm::max(mRaytracerRequirements.mBLASCollectionTexels_Width, mRaytracerRequirements.mBLASCollectionTexels_Height)) &&
			(deviceProperties.limits.maxImageDimension3D >= glm::max(glm::max(mRaytracerRequirements.mGeometryCollectionTexels_Width, mRaytracerRequirements.mGeometryCollectionTexels_Height), mRaytracerRequirements.mGeometryCollectionTexels_Depth)) &&
			(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {

			DBG_ONLY(	printf("Chosen Vulkan GPU: %s\n\n", deviceProperties.deviceName ) );

			mPhysicalDevice = physicalDevice;
			deviceFound = true;
			break;
		}
	}

	DBG_ASSERT(deviceFound); // No suitable Vulkan device found
}

void VulkanPipeline::createInstance() noexcept {
	/*
	By enabling validation layers, Vulkan will emit warnings if the API
	is used incorrectly. We shall enable the layer VK_LAYER_LUNARG_standard_validation,
	which is basically a collection of several useful validation layers.
	*/
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
	/*
	We get all supported layers with vkEnumerateInstanceLayerProperties.
	*/
	glm::uint32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	std::vector<VkLayerProperties> layerProperties(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	/*
	And then we simply check if VK_LAYER_LUNARG_standard_validation is among the supported layers.
	*/
	bool foundLayer = false;
	for (VkLayerProperties prop : layerProperties) {
		if (strcmp("VK_LAYER_LUNARG_standard_validation", prop.layerName) == 0) {
			foundLayer = true;
			break;
		}
	}

	DBG_ASSERT(foundLayer); // Layer VK_LAYER_LUNARG_standard_validation not supported

	mEnabledLayers.push_back("VK_LAYER_LUNARG_standard_validation"); // Alright, we can use this layer.

	/*
	We need to enable an extension named VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
	in order to be able to print the warnings emitted by the validation layer.
	So again, we just check if the extension is among the supported extensions.
	*/

	glm::uint32 extensionCount;

	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProperties.data());

	bool foundExtension = false;
	for (VkExtensionProperties prop : extensionProperties) {
		if (strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, prop.extensionName) == 0) {
			foundExtension = true;
			break;
		}
	}

	DBG_ASSERT(foundExtension); // Extension VK_EXT_DEBUG_REPORT_EXTENSION_NAME not supported

	mEnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	/*
	Contains application info. This is actually not that important.
	The only real important field is apiVersion.
	*/
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Hello world app";
	applicationInfo.applicationVersion = 0;
	applicationInfo.pEngineName = "Tachyon";
	applicationInfo.engineVersion = 0;
	applicationInfo.apiVersion = VK_API_VERSION_1_0;;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &applicationInfo;

	// Give our desired layers and extensions to vulkan.
	createInfo.enabledLayerCount = mEnabledLayers.size();
	createInfo.ppEnabledLayerNames = mEnabledLayers.data();
	createInfo.enabledExtensionCount = mEnabledExtensions.size();
	createInfo.ppEnabledExtensionNames = mEnabledExtensions.data();

	// TODO: the second parameter specify a custom memory allocator to be used within the created instance
	VK_CHECK_RESULT(vkCreateInstance(&createInfo, NULL, &mInstance));

	/*
	Register a callback function for the extension VK_EXT_DEBUG_REPORT_EXTENSION_NAME, so that warnings emitted from the validation
	layer are actually printed.
	*/

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
	VkDebugReportCallbackCreateInfoEXT createReportInfo = {};
	createReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createReportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	createReportInfo.pfnCallback = &debugReportCallbackFn;

	// We have to explicitly load this function.
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT");
	
	DBG_ASSERT((vkCreateDebugReportCallbackEXT != nullptr) ); // Could not load vkCreateDebugReportCallbackEXT

	// Create and register callback.
	VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(mInstance, &createReportInfo, NULL, &debugReportCallback));
#endif
}

void VulkanPipeline::createLogicalDevice() noexcept {
	// When creating the device, we also specify what queues it has.
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	mQueueFamilyIndex = getQueueFamilyIndex(); // find queue family with all required capability.
	queueCreateInfo.queueFamilyIndex = mQueueFamilyIndex;
	queueCreateInfo.queueCount = 1; // create one queue in this family. We don't need more.
	float queuePriorities = 1.0;  // we only have one queue, so this is not that imporant. 
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	//Now we create the logical device. The logical device allows us to interact with the physical device.
	VkDeviceCreateInfo deviceCreateInfo = {};

	// Specify any desired device features here. We do not need any for this application, though.
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.enabledLayerCount = mEnabledLayers.size();  // need to specify validation layers here as well.
	deviceCreateInfo.ppEnabledLayerNames = mEnabledLayers.data();
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; // when creating the logical device, we also specify what queues it has.
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VK_CHECK_RESULT(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, NULL, &mDevice)); // create logical device.

	// Get a handle to the only member of the queue family.
	vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mQueue);
}

// Returns the index of a queue family that supports compute operations. 
glm::uint32 VulkanPipeline::getQueueFamilyIndex() noexcept {
	uint32_t queueFamilyCount;

	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, NULL);

	// Retrieve all queue families.
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	// Now find a family that supports compute.
	uint32_t i = 0;
	for (; i < queueFamilies.size(); ++i) {
		VkQueueFamilyProperties props = queueFamilies[i];

		if (props.queueCount > 0 && (
			(props.queueFlags & VK_QUEUE_COMPUTE_BIT) && 
			(props.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
			(props.queueFlags & VK_QUEUE_TRANSFER_BIT)
			)) {
			// found a candidate!
			break;
		}
	}

	DBG_ASSERT((i != queueFamilies.size())); // could not find a queue family that supports required operations

	return i;
}

uint32_t VulkanPipeline::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) noexcept {
	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memoryProperties);

	// See the documentation of VkPhysicalDeviceMemoryProperties for a detailed description.
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((memoryTypeBits & (1 << i)) &&
			((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}

	DBG_ASSERT( false ); // out-of-memory or to enough VRAM

	return -1;
}

void VulkanPipeline::allocateBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize memorySize, VkBufferUsageFlags memoryUsage, VkMemoryPropertyFlags memoryProperties) noexcept {
	// This is to create the TLAS buffer memory
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = memorySize; // buffer size in bytes
	bufferCreateInfo.usage = memoryUsage; // buffer is used as an image (mainly because I hope to get better performance and keep compatibility with OpenGL)
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // buffer is exclusive to a single queue family at a time. 

	VK_CHECK_RESULT(vkCreateBuffer(mDevice, &bufferCreateInfo, NULL, &buffer)); // create buffer.

	//!!! But the buffer doesn't allocate memory for itself, so we must do that manually !!!

	//First, we find the memory requirements for the buffer.
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(mDevice, buffer, &memoryRequirements);

	//Now use obtained memory requirements info to allocate the memory for the buffer.
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size; // specify required memory.

	/*
	There are several types of memory that can be allocated, and we must choose a memory type that:
	1) Satisfies the memory requirements(memoryRequirements.memoryTypeBits).
	2) Satifies our own usage requirements.
	
	To satisfy condition two we can set a bitwise combination of following flags:
		- VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: memory can be mapped with vkMapMemory
		- VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: memory written by the device(GPU) will be easily visible to the host(CPU), without having to call any extra flushing commands.
	*/
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, memoryProperties);

	VK_CHECK_RESULT(vkAllocateMemory(mDevice, &allocateInfo, NULL, &bufferMemory)); // allocate memory on device.

	// Now associate that allocated memory with the buffer. With that, the buffer is backed by actual memory.
	VK_CHECK_RESULT(vkBindBufferMemory(mDevice, buffer, bufferMemory, 0));
}

void VulkanPipeline::destroyBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory) noexcept {
	// Destroy TLAS buffer memory
	vkFreeMemory(mDevice, bufferMemory, NULL);
	vkDestroyBuffer(mDevice, buffer, NULL);
}

void VulkanPipeline::createCoreBuffers() noexcept {
	VkImageCreateInfo* tlasImageCreateInfo = new VkImageCreateInfo(); // Using stack will lead to stack overflow
	tlasImageCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	tlasImageCreateInfo->pNext = NULL;
	tlasImageCreateInfo->flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	tlasImageCreateInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
	tlasImageCreateInfo->samples = VK_SAMPLE_COUNT_1_BIT;
	tlasImageCreateInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
	tlasImageCreateInfo->usage = VK_IMAGE_USAGE_STORAGE_BIT;
	tlasImageCreateInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	tlasImageCreateInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	tlasImageCreateInfo->mipLevels = 1;
	tlasImageCreateInfo->arrayLayers = 1;
	tlasImageCreateInfo->imageType = VK_IMAGE_TYPE_1D;
	tlasImageCreateInfo->extent.width = mRaytracerRequirements.mTLASTexels_Width;
	tlasImageCreateInfo->extent.height = 1;
	tlasImageCreateInfo->extent.depth = 1;

	vkCreateImage(mDevice, tlasImageCreateInfo, NULL, &mRaytracingTLAS);
	
	// BLAS-Collection specific
	VkImageCreateInfo* blasImageCreateInfo = new VkImageCreateInfo(); // Using stack will lead to stack overflow
	blasImageCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	blasImageCreateInfo->pNext = NULL;
	blasImageCreateInfo->flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	blasImageCreateInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
	blasImageCreateInfo->samples = VK_SAMPLE_COUNT_1_BIT;
	blasImageCreateInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
	blasImageCreateInfo->usage = VK_IMAGE_USAGE_STORAGE_BIT;
	blasImageCreateInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	blasImageCreateInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	blasImageCreateInfo->mipLevels = 1;
	blasImageCreateInfo->arrayLayers = 1;
	blasImageCreateInfo->imageType = VK_IMAGE_TYPE_2D;
	blasImageCreateInfo->extent.width = mRaytracerRequirements.mBLASCollectionTexels_Width;
	blasImageCreateInfo->extent.height = mRaytracerRequirements.mBLASCollectionTexels_Height;
	blasImageCreateInfo->extent.depth = 1;
	
	// This is to create the BLAS-Collection
	vkCreateImage(mDevice, blasImageCreateInfo, NULL, &mRaytracingBLASCollection);

	// ModelMatrix-Collection specific
	VkImageCreateInfo* modelMatrixImageCreateInfo = new VkImageCreateInfo(); // Using stack will lead to stack overflow
	modelMatrixImageCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	modelMatrixImageCreateInfo->pNext = NULL;
	modelMatrixImageCreateInfo->flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	modelMatrixImageCreateInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
	modelMatrixImageCreateInfo->samples = VK_SAMPLE_COUNT_1_BIT;
	modelMatrixImageCreateInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
	modelMatrixImageCreateInfo->usage = VK_IMAGE_USAGE_STORAGE_BIT;
	modelMatrixImageCreateInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	modelMatrixImageCreateInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	modelMatrixImageCreateInfo->mipLevels = 1;
	modelMatrixImageCreateInfo->arrayLayers = 1;
	modelMatrixImageCreateInfo->imageType = VK_IMAGE_TYPE_2D;
	modelMatrixImageCreateInfo->extent.width = mRaytracerRequirements.mModelMatrixCollection_Width;
	modelMatrixImageCreateInfo->extent.height = mRaytracerRequirements.mModelMatrixCollection_Height;
	modelMatrixImageCreateInfo->extent.depth = 1;

	// This is to create the ModelMatrix-Collection
	vkCreateImage(mDevice, modelMatrixImageCreateInfo, NULL, &mRaytracingModelMatrix);

	// ModelMatrix-Collection specific
	VkImageCreateInfo* geometryImageCreateInfo = new VkImageCreateInfo(); // Using stack will lead to stack overflow
	geometryImageCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	geometryImageCreateInfo->pNext = NULL;
	geometryImageCreateInfo->flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	geometryImageCreateInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
	geometryImageCreateInfo->samples = VK_SAMPLE_COUNT_1_BIT;
	geometryImageCreateInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
	geometryImageCreateInfo->usage = VK_IMAGE_USAGE_STORAGE_BIT;
	geometryImageCreateInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	geometryImageCreateInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	geometryImageCreateInfo->mipLevels = 1;
	geometryImageCreateInfo->arrayLayers = 1;
	geometryImageCreateInfo->imageType = VK_IMAGE_TYPE_3D;
	geometryImageCreateInfo->extent.width = mRaytracerRequirements.mGeometryCollectionTexels_Width;
	geometryImageCreateInfo->extent.height = mRaytracerRequirements.mGeometryCollectionTexels_Height;
	geometryImageCreateInfo->extent.depth = mRaytracerRequirements.mGeometryCollectionTexels_Depth;

	// This is to create the ModelMatrix-Collection
	vkCreateImage(mDevice, geometryImageCreateInfo, NULL, &mRaytracingGeometryCollection);

	delete tlasImageCreateInfo;
	delete blasImageCreateInfo;
	delete modelMatrixImageCreateInfo;
	delete geometryImageCreateInfo;

	// Find memory requirements for all those images used as a buffer
	VkMemoryRequirements raytracerTLASMemoryRequirements;
	vkGetImageMemoryRequirements(mDevice, mRaytracingTLAS, &raytracerTLASMemoryRequirements);
	VkMemoryRequirements raytracerBLASCollectionemoryRequirements;
	vkGetImageMemoryRequirements(mDevice, mRaytracingBLASCollection, &raytracerBLASCollectionemoryRequirements);
	VkMemoryRequirements raytracerModelMatrixCollectionemoryRequirements;
	vkGetImageMemoryRequirements(mDevice, mRaytracingModelMatrix, &raytracerModelMatrixCollectionemoryRequirements);
	VkMemoryRequirements raytracerGeometryCollectionemoryRequirements;
	vkGetImageMemoryRequirements(mDevice, mRaytracingGeometryCollection, &raytracerGeometryCollectionemoryRequirements);

	// This is the space (in bytes) needed to store core buffers in a worst case scenario of alignment 
	size_t global_core_buffers_size = (raytracerTLASMemoryRequirements.size + 2*raytracerTLASMemoryRequirements.alignment) +
		(raytracerBLASCollectionemoryRequirements.size + raytracerBLASCollectionemoryRequirements.alignment) +
		(raytracerModelMatrixCollectionemoryRequirements.size + raytracerModelMatrixCollectionemoryRequirements.alignment) +
		(raytracerGeometryCollectionemoryRequirements.size + raytracerGeometryCollectionemoryRequirements.alignment);
	size_t remaining = global_core_buffers_size;

	// Memory must be aligned
	void* ptr = (void*)(uintptr_t(1));
	VkDeviceSize mRaytracingTLASCollectionOffset = reinterpret_cast<intptr_t>(std::align(raytracerTLASMemoryRequirements.alignment, raytracerTLASMemoryRequirements.size, ptr, remaining));
	DBG_ASSERT((mRaytracingTLASCollectionOffset != 0));
	VkDeviceSize mRaytracingBLASCollectionOffset = reinterpret_cast<intptr_t>(std::align(raytracerBLASCollectionemoryRequirements.alignment, raytracerBLASCollectionemoryRequirements.size, ptr, remaining));
	DBG_ASSERT((mRaytracingBLASCollectionOffset != 0));
	VkDeviceSize mRaytracingModelMatrixCollectionOffset = reinterpret_cast<intptr_t>(std::align(raytracerModelMatrixCollectionemoryRequirements.alignment, raytracerModelMatrixCollectionemoryRequirements.size, ptr, remaining));
	DBG_ASSERT((mRaytracingModelMatrixCollectionOffset != 0));
	VkDeviceSize mRaytracingGeometryCollectionOffset = reinterpret_cast<intptr_t>(std::align(raytracerGeometryCollectionemoryRequirements.alignment, raytracerGeometryCollectionemoryRequirements.size, ptr, remaining));
	DBG_ASSERT((mRaytracingGeometryCollectionOffset != 0));
	DBG_ASSERT((remaining >= 0));

	//Now use obtained memory requirements info to allocate the memory for the buffer.
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = global_core_buffers_size;
	allocateInfo.memoryTypeIndex = findMemoryType(raytracerTLASMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Allocate a big chunk of memory on the device
	VK_CHECK_RESULT(vkAllocateMemory(mDevice, &allocateInfo, NULL, &mRaytracingDeviceMemory));
	
	// Associate each image with a slice of the allocated memory
	VK_CHECK_RESULT(vkBindImageMemory(mDevice, mRaytracingTLAS, mRaytracingDeviceMemory, mRaytracingTLASCollectionOffset));
	VK_CHECK_RESULT(vkBindImageMemory(mDevice, mRaytracingBLASCollection, mRaytracingDeviceMemory, mRaytracingBLASCollectionOffset));
	VK_CHECK_RESULT(vkBindImageMemory(mDevice, mRaytracingModelMatrix, mRaytracingDeviceMemory, mRaytracingModelMatrixCollectionOffset));
	VK_CHECK_RESULT(vkBindImageMemory(mDevice, mRaytracingGeometryCollection, mRaytracingDeviceMemory, mRaytracingGeometryCollectionOffset));
}

void VulkanPipeline::destroyCoreBuffers() noexcept {
	// Destroy the memory backing-up all core images used to store raytracing input
	vkFreeMemory(mDevice, mRaytracingDeviceMemory, NULL);

	// Destroy all images
	vkDestroyImage(mDevice, mRaytracingTLAS, NULL);
	vkDestroyImage(mDevice, mRaytracingBLASCollection, NULL);
	vkDestroyImage(mDevice, mRaytracingModelMatrix, NULL);
}

void VulkanPipeline::createPipeline() noexcept {
	VkShaderModuleCreateInfo insertModuleCreation;
	insertModuleCreation.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	insertModuleCreation.pNext = NULL;
	//insertModuleCreation.codeSize = ;
	//insertModuleCreation.pCode = ;

}

void VulkanPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept {

}

void VulkanPipeline::onReset() noexcept {

}

void VulkanPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {

}

void VulkanPipeline::onRender(const Core::Camera& camera) noexcept {

}