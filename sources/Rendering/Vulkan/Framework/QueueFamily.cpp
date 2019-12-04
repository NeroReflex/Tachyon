#include "QueueFamily.h"
#include "Queue.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

QueueFamily::QueueFamily(const Device* const device, std::vector<QueueFamilySupportedOperationType> supportedOperationTypes, uint32_t familyIndex) noexcept
	: DeviceOwned(device), mSupportedOperationTypes(std::move(supportedOperationTypes)), mFamilyIndex(std::move(familyIndex)) {}

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
	return mSupportedOperationTypes;
}
