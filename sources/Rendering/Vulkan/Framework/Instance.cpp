#include "Instance.h"
#include "InstanceOwned.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

const float Instance::defaultQueuePriority = 1.0;

Instance::Instance(GLFWwindow* window) noexcept 
	: mInstanceExtensionsCount(0),
	mInstanceExtensions(nullptr),
	mDeviceExtensionsCount(0),
	mDeviceExtensions(nullptr) {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Query GLFW-required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	// When creating a device it is important to specify the VK_KHR_swapchain extension or the device cannot be used for rendering
	enqueueDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// Import all GLFW extensions to the safe buffer
	for (uint32_t j = 0; j < glfwExtensionCount; ++j) enqueueInstanceExtension(glfwExtensions[j]);
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	mValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = mInstanceExtensionsCount;
	createInfo.ppEnabledExtensionNames = mInstanceExtensions;
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
#else
	{ // Disable validation layers
		createInfo.enabledLayerCount = 0;
	}
#endif

	VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &mInstance));

	VK_CHECK_RESULT(glfwCreateWindowSurface(mInstance, window, NULL, &mSurface));
}

Instance::~Instance() {
	for (auto& ownedObj : mObjectsCollection)
		ownedObj.reset();

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

	vkDestroyInstance(mInstance, nullptr);

	for (uint32_t j = 0; j < mDeviceExtensionsCount; ++j) delete mDeviceExtensions[j];
	delete[] mDeviceExtensions;

	for (uint32_t j = 0;  j < mInstanceExtensionsCount; ++j) delete mInstanceExtensions[j];
	delete[] mInstanceExtensions;
}

void Instance::enqueueInstanceExtension(const char* extName) noexcept {
	mInstanceExtensionsCount += 1;

	char** prexExtList = mInstanceExtensions;
	mInstanceExtensions = new char*[mInstanceExtensionsCount];
	std::memcpy(mInstanceExtensions, prexExtList, sizeof(char*) * (mInstanceExtensionsCount - 1));
	mInstanceExtensions[mInstanceExtensionsCount - 1] = new char[256];
	std::strcpy(mInstanceExtensions[mInstanceExtensionsCount - 1], extName);
	delete[] prexExtList;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	DBG_ONLY((std::cout << "Enabled instance extension: \"" << mInstanceExtensions[mInstanceExtensionsCount - 1] << "\"" << std::endl));
#endif
}

void Instance::enqueueDeviceExtension(const char* extName) noexcept {
	mDeviceExtensionsCount += 1;

	char** prexExtList = mDeviceExtensions;
	mDeviceExtensions = new char* [mDeviceExtensionsCount];
	std::memcpy(mDeviceExtensions, prexExtList, sizeof(char*) * (mDeviceExtensionsCount - 1));
	mDeviceExtensions[mDeviceExtensionsCount - 1] = new char[256];
	std::strcpy(mDeviceExtensions[mDeviceExtensionsCount - 1], extName);
	delete[] prexExtList;

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	DBG_ONLY((std::cout << "Enabled device extension: \"" << mDeviceExtensions[mDeviceExtensionsCount - 1] << "\"" << std::endl));
#endif
}

std::vector<VkExtensionProperties> Instance::getAllSupportedExtensions() noexcept {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	return extensions;
}

const VkInstance& Instance::getNativeInstanceHandle() const noexcept
{
	return mInstance;
}

bool Instance::corresponds(const std::vector<QueueFamily::QueueFamilySupportedOperationType>& operations, VkQueueFamilyProperties queueFamily, VkPhysicalDevice device, uint32_t familyIndex) const noexcept {
	bool featureRequested = true;
	bool featureFound = false;
	
	auto opIt = std::find(operations.cbegin(), operations.cend(), QueueFamily::QueueFamilySupportedOperationType::Transfer);
	featureRequested = (opIt != operations.cend());
	featureFound = queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT;
	if (featureFound != featureRequested) return false;
	
	opIt = std::find(operations.cbegin(), operations.cend(), QueueFamily::QueueFamilySupportedOperationType::Compute);
	featureRequested = (opIt != operations.cend());
	featureFound = queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
	if (featureFound != featureRequested) return false;
	
	opIt = std::find(operations.cbegin(), operations.cend(), QueueFamily::QueueFamilySupportedOperationType::Graphics);
	featureRequested = (opIt != operations.cend());
	featureFound = queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	if (featureFound != featureRequested) return false;
	
	opIt = std::find(operations.cbegin(), operations.cend(), QueueFamily::QueueFamilySupportedOperationType::Present);
	featureRequested = (opIt != operations.cend());
	if ((featureRequested) && (!glfwGetPhysicalDevicePresentationSupport(mInstance, device, familyIndex))) return false;
	
	return true;
}

Device* Instance::openDevice(std::vector<QueueFamily::ConcreteQueueFamilyDescriptor> queueDescriptors) noexcept {
	DBG_ASSERT((queueDescriptors.size() > 0));

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	uint64_t bestPhysicalDeviceScore = 0;
	VkPhysicalDevice bestPhysicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceFeatures bestPhysicalDeviceFeatures;

	for (const VkPhysicalDevice& physicalDevice : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		uint64_t MSBytes = 0;
		
		switch (deviceProperties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				MSBytes = 0xC000000000000000;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				MSBytes = 0x8000000000000000;

			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				MSBytes = 0x4000000000000000;

			default:
				MSBytes = 0x0000000000000000;
		}

		uint64_t currentScore = MSBytes | deviceProperties.limits.maxImageDimension3D;

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) { // Exclude inadequate devices
			currentScore = 0;
		}

		if (bestPhysicalDeviceScore < currentScore) {
			bestPhysicalDevice = physicalDevice;
			bestPhysicalDeviceFeatures = deviceFeatures;
		}

	}

	DBG_ASSERT((bestPhysicalDevice != VK_NULL_HANDLE));

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(bestPhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(bestPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	const auto requestedDescriptors = queueDescriptors;
	
	std::vector<VkDeviceQueueCreateInfo> selectedQueues;
	std::vector<std::tuple<QueueFamily::ConcreteQueueFamilyDescriptor, uint32_t>> requiredQueueFamilyCollection;

	while (!queueDescriptors.empty()) {
		const auto currentDescriptorIt = queueDescriptors.cbegin();
		
		uint32_t familyIndex = 0;
		for (const auto& queueFamily : queueFamilies) {

			DBG_ASSERT((currentDescriptorIt->maxQueues > 0));

			if (corresponds(currentDescriptorIt->supportedOperations, queueFamily, bestPhysicalDevice, familyIndex)) {
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = familyIndex;
				queueCreateInfo.queueCount = currentDescriptorIt->maxQueues;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;

				selectedQueues.emplace_back(std::move(queueCreateInfo));

				requiredQueueFamilyCollection.emplace_back(std::make_tuple(*currentDescriptorIt, familyIndex));

				break; // found a suitable queue. Stop the search.
			}
			
			familyIndex++;
		}
		
		queueDescriptors.erase(currentDescriptorIt);
	}

	DBG_ASSERT(requestedDescriptors.size() == selectedQueues.size());


	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = selectedQueues.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(selectedQueues.size());
	createInfo.pEnabledFeatures = &bestPhysicalDeviceFeatures;
	createInfo.enabledExtensionCount = mDeviceExtensionsCount;
	createInfo.ppEnabledExtensionNames = mDeviceExtensions;

	/*
	Previous implementations of Vulkan made a distinction between instance and device specific validation layers, but this is no longer the case.
	That means that the enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by up-to-date implementations.
	However, it is still a good idea to set them anyway to be compatible with older implementations:
	*/
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
#else
	{
		createInfo.enabledLayerCount = 0;
	}
#endif

	VkDevice newDevice;
	VK_CHECK_RESULT(vkCreateDevice(bestPhysicalDevice, &createInfo, nullptr, &newDevice));

	Device* managedDeviceHandle = new Device(this, std::move(bestPhysicalDevice), std::move(newDevice), std::move(requiredQueueFamilyCollection));

	mObjectsCollection.emplace_back(managedDeviceHandle);

	return managedDeviceHandle;
}

const VkSurfaceKHR& Instance::getSurface() const noexcept {
	return mSurface;
}
