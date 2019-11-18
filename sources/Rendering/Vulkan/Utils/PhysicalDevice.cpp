#include "PhysicalDevice.h"
#include "Surface.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

PhysicalDevice::PhysicalDevice(const std::shared_ptr<Instance>& instance, VkPhysicalDevice physicalDevice, std::optional<VkSurfaceKHR> surface) noexcept
	: InstanceObject(instance),
	mPhysicalDevice(std::move(physicalDevice)),
	mSurface(surface.has_value() ? std::make_shared<Surface>(instance, surface.value()) : std::shared_ptr<Surface>(nullptr)) {
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mPhysicalDeviceFeatures);
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);
	
	if (surface.has_value()) {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, surface.value(), &mPhysicalDeviceSwapchainCapabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface.value(), &formatCount, nullptr);

		if (formatCount != 0) {
			mPhysicalDeviceSwapchainFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface.value(), &formatCount, mPhysicalDeviceSwapchainFormats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, surface.value(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			mPhysicalDeviceSwapchainPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, surface.value(), &presentModeCount, mPhysicalDeviceSwapchainPresentModes.data());
		}
	}
}

PhysicalDevice::~PhysicalDevice() {}

std::shared_ptr<Device> PhysicalDevice::selectQueueFamily(const QueueFamilySelector& selector) const noexcept {
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, NULL);

	// Retrieve all queue families.
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	VkDevice dev;
	VkQueue queue;

	// Now find a family that supports compute.
	for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
		const VkQueueFamilyProperties& props = queueFamilies[i];

		if (selector(getInstance()->getNativeHandle(), this->getNativeHandle(), i, props)) {
			// When creating the device, we also specify what queues it has.
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.queueCount = 1; // create one queue in this family. We don't need more.
			float queuePriorities = 1.0;  // we only have one queue, so this is not that imporant. 
			queueCreateInfo.pQueuePriorities = &queuePriorities;

			//Now we create the logical device. The logical device allows us to interact with the physical device.
			VkDeviceCreateInfo deviceCreateInfo = {};

			// Specify any desired device features here. We do not need any for this application, though.
			VkPhysicalDeviceFeatures deviceFeatures = {};

			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(getInstanceEnabledValidationLayers().size());  // need to specify validation layers here as well.
			deviceCreateInfo.ppEnabledLayerNames = getInstanceEnabledValidationLayers().data();
			deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; // when creating the logical device, we also specify what queues it has.
			deviceCreateInfo.queueCreateInfoCount = 1;
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

			VK_CHECK_RESULT(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, NULL, &dev)); // create logical device.

			// Get a handle to the only member of the queue family.
			vkGetDeviceQueue(dev, i, 0, &queue);

			return std::make_shared<Device>(getInstance(), std::move(dev), std::move(queue));
		}
	}

	DBG_ASSERT(false);

	return nullptr;
}

const VkPhysicalDevice& PhysicalDevice::getNativeHandle() const noexcept {
	return mPhysicalDevice;
}
