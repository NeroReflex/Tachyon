#include "QueueFamily.h"
#include "Queue.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

QueueFamily::ConcreteQueueFamilyDescriptor::ConcreteQueueFamilyDescriptor(QueueFamily::QueueFamilySelection supportedOperations, uint32_t maxQueues) noexcept
	: supportedOperations(std::move(supportedOperations)), maxQueues(std::move(maxQueues)) {}

QueueFamily::QueueFamily(Device* const device, const ConcreteQueueFamilyDescriptor& familyDescriptor, uint32_t familyIndex) noexcept
	: DeviceOwned(device), mQueueFamilyDescriptor(familyDescriptor), mFamilyIndex(std::move(familyIndex)) {

	// Open all queues
	for (uint32_t i = 0; i < mQueueFamilyDescriptor.maxQueues; ++i) {
		VkQueue queue;
		vkGetDeviceQueue(getParentDevice()->getNativeDeviceHandle(), mFamilyIndex, i, &queue);

		new Queue(this, std::move(queue));
	}

}

QueueFamily::~QueueFamily() {
	while (!mQueues.empty()) {
		auto allocatedDescritorSet = mQueues.begin();
		allocatedDescritorSet->second.reset(nullptr);
	}
	mQueues.clear();
}

uint32_t QueueFamily::getNativeQueueFamilyIndexHandle() const noexcept {
	return mFamilyIndex;
}

const std::vector<QueueFamily::QueueFamilySupportedOperationType>& QueueFamily::getSupportedOperationTypes() const noexcept {
	return mQueueFamilyDescriptor.supportedOperations;
}

uint32_t QueueFamily::getMaximumQueuesCount() const noexcept {
	return mQueueFamilyDescriptor.maxQueues;
}

Queue* QueueFamily::getQueue(uint32_t index) const noexcept {
	DBG_ASSERT((index < getMaximumQueuesCount()));

	auto queueIt = mQueues.cbegin();
	std::advance(queueIt, index);

	return queueIt->second.get();
}