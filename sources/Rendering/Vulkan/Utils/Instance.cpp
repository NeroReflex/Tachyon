#include "Instance.h"
#include "PhysicalDevice.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

Instance::PhysicalDeviceSupportingExtensionsSelector::PhysicalDeviceSupportingExtensionsSelector(std::vector<const char*> extensions) noexcept
	: mRequiredExtensions(extensions) {}

bool Instance::PhysicalDeviceSupportingExtensionsSelector::operator()(VkPhysicalDevice physicalDevice) const noexcept {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	// TODO: move this out
	std::set<std::string> requiredExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Instance::Instance(VkInstance instance) noexcept
	: mInstance(instance) {}

Instance::Instance(PFN_vkDebugReportCallbackEXT debugReportCallbackFn, const std::vector<std::string>& requiredExtensions) noexcept
	: mEnabledExtensions()
{

	for (const std::string& extName: requiredExtensions) {
		mEnabledExtensions.push_back(extName.c_str());
	}

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
	applicationInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &applicationInfo;

	// Give our desired layers and extensions to vulkan.
	createInfo.enabledLayerCount = static_cast<uint32_t>(mEnabledLayers.size());
	createInfo.ppEnabledLayerNames = mEnabledLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(mEnabledExtensions.size());
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
	createReportInfo.pfnCallback = debugReportCallbackFn;

	// We have to explicitly load this function.
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT");

	DBG_ASSERT((vkCreateDebugReportCallbackEXT != nullptr)); // Could not load vkCreateDebugReportCallbackEXT

	// Create and register callback.
	VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(mInstance, &createReportInfo, NULL, &debugReportCallback));
#endif
}

Instance::~Instance() {
#if defined(VULKAN_ENABLE_VALIDATION_LAYERS)
	// destroy callback.
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT");

	DBG_ASSERT(func != nullptr); // Could not load vkDestroyDebugReportCallbackEXT

	func(mInstance, debugReportCallback, NULL);
#endif

	vkDestroyInstance(mInstance, NULL);
}

const VkInstance& Instance::getNativeHandle() const noexcept
{
	return mInstance;
}

std::shared_ptr<PhysicalDevice> Instance::selectPhysicalDevice(const PhysicalDeviceSelector& selector, std::optional<VkSurfaceKHR> surface) const noexcept {
	//list all physical devices on the system with vkEnumeratePhysicalDevices
	glm::uint32 deviceCount;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, NULL);

	DBG_ASSERT((deviceCount > 0)); // could not find a device with Vulkan support

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	uint64_t bestScore = 0;
	VkPhysicalDevice bestDevice;
	for (VkPhysicalDevice physicalDevice : devices) {
		if (selector(physicalDevice)) {
			uint64_t currentDeviceScore = 1;

			if (currentDeviceScore > bestScore) {
				bestDevice = physicalDevice;
			}
		}
	}

	return (bestScore > 0) ? std::make_shared<PhysicalDevice>(this->shared_from_this(), std::move(bestDevice), surface) : std::shared_ptr<PhysicalDevice>(nullptr);
}
