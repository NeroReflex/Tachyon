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
}

VulkanPipeline::~VulkanPipeline() {
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
		// destroy callback.
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT");

		DBG_ASSERT(func != nullptr); // Could not load vkDestroyDebugReportCallbackEXT

		func(mInstance, debugReportCallback, NULL);
#endif

	/*vkFreeMemory(device, bufferMemory, NULL);
	vkDestroyBuffer(device, buffer, NULL);
	vkDestroyShaderModule(device, computeShaderModule, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyCommandPool(mDevice, commandPool, NULL);*/
	vkDestroyDevice(mDevice, NULL);
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
			(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {

			DBG_ONLY(	printf("Chosen GPU: %s", deviceProperties.deviceName ) );

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
			// found a queue with compute. We're done!
			break;
		}
	}

	DBG_ASSERT((i != queueFamilies.size())); // could not find a queue family that supports required operations

	return i;
}

void VulkanPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept {

}

void VulkanPipeline::onReset() noexcept {

}

void VulkanPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {

}

void VulkanPipeline::onRender(const Core::Camera& camera) noexcept {

}