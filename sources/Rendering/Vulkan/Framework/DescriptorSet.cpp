#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DescriptorSet::DescriptorSet(DescriptorPool* const pool, VkDescriptorSet&& descriptorPool) noexcept
	: mParentDescriptorPool(pool),
	mDescriptorSet(std::move(descriptorPool)) {
	getParentDescriptorPool()->mAllocatedDescriptorSets.emplace(std::pair<uintptr_t, std::unique_ptr<DescriptorSet>>(uintptr_t(this), this));
}

DescriptorSet::~DescriptorSet() {
	const auto selfIt = getParentDescriptorPool()->mAllocatedDescriptorSets.find(uintptr_t(this));
	if (selfIt != getParentDescriptorPool()->mAllocatedDescriptorSets.cend()) {
		getParentDescriptorPool()->mAllocatedDescriptorSets.erase(selfIt);
	}

	vkFreeDescriptorSets(getParentDescriptorPool()->getParentDevice()->getNativeDeviceHandle(), getParentDescriptorPool()->getNativeDescriptorPoolHandle(), 1, &mDescriptorSet);
}

const VkDescriptorSet& DescriptorSet::getNativeDescriptorSetHandle() const noexcept {
	return mDescriptorSet;
}

DescriptorPool* DescriptorSet::getParentDescriptorPool() const noexcept {
	return mParentDescriptorPool;
}

void DescriptorSet::bindBuffer() const noexcept {

}

void DescriptorSet::bindImages(uint32_t firstLayoutId, std::initializer_list<const ImageInterface*> images) const noexcept {
	std::vector<VkDescriptorImageInfo> descriptors;
	
	for (const auto& image : images) {
		VkDescriptorImageInfo currentImageDescriptor;

		descriptors.emplace_back(std::move(currentImageDescriptor));
	}

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = mDescriptorSet;
	writeDescriptorSet.dstBinding = firstLayoutId;
	writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptors.size());
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSet.pImageInfo = descriptors.data();
}
