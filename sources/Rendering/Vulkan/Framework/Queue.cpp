#include "Queue.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Queue::Queue(QueueFamily* const queueFamily, VkQueue&& queue) noexcept
	: mQueueFamily(queueFamily), mQueue(std::move(queue)) {

	// self-register to the queue list
	mQueueFamily->mQueues.emplace(std::pair<uintptr_t, std::unique_ptr<Queue>>(uintptr_t(this), this));
}

Queue::~Queue() {
	const auto selfIt = getParentQueueFamily()->mQueues.find(uintptr_t(this));
	if (selfIt != getParentQueueFamily()->mQueues.cend()) {
		getParentQueueFamily()->mQueues.erase(selfIt);
	}

	// TODO: destroy the queue
}

QueueFamily* Queue::getParentQueueFamily() const noexcept {
	return mQueueFamily;
}

const VkQueue& Queue::getNativeQueueHandle() const noexcept {
	return mQueue;
}
