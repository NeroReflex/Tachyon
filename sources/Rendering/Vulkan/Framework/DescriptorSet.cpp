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

void DescriptorSet::bindUniformBuffers(uint32_t firstLayoutId, const std::initializer_list<const Buffer*>& buffers) const noexcept {
	std::vector<VkDescriptorBufferInfo> descriptors;

	for (const auto& buffer : buffers) {
		DBG_ASSERT( (buffer->getBufferUsage() & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) );

		VkDescriptorBufferInfo currentBufferDescriptor;
		currentBufferDescriptor.buffer = buffer->getNativeBufferHandle();
		currentBufferDescriptor.offset = 0;
		currentBufferDescriptor.range = VK_WHOLE_SIZE;

		descriptors.emplace_back(std::move(currentBufferDescriptor));
	}

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = mDescriptorSet;
	writeDescriptorSet.dstBinding = firstLayoutId;
	writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptors.size());
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = descriptors.data();

	vkUpdateDescriptorSets(getParentDescriptorPool()->getParentDevice()->getNativeDeviceHandle(), 1, &writeDescriptorSet, 0, NULL);
}

void DescriptorSet::bindStorageBuffers(uint32_t firstLayoutId, const std::initializer_list<const Buffer*>& buffers) const noexcept {
	std::vector<VkDescriptorBufferInfo> descriptors;

	for (const auto& buffer : buffers) {
		DBG_ASSERT((buffer->getBufferUsage() & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));

		VkDescriptorBufferInfo currentBufferDescriptor;
		currentBufferDescriptor.buffer = buffer->getNativeBufferHandle();
		currentBufferDescriptor.offset = 0;
		currentBufferDescriptor.range = VK_WHOLE_SIZE;

		descriptors.emplace_back(std::move(currentBufferDescriptor));
	}

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = mDescriptorSet;
	writeDescriptorSet.dstBinding = firstLayoutId;
	writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptors.size());
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writeDescriptorSet.pBufferInfo = descriptors.data();

	vkUpdateDescriptorSets(getParentDescriptorPool()->getParentDevice()->getNativeDeviceHandle(), 1, &writeDescriptorSet, 0, NULL);
}

void DescriptorSet::bindImages(uint32_t firstLayoutId, std::initializer_list<std::tuple<VkImageLayout, const ImageView*>> images) const noexcept {
	std::vector<VkDescriptorImageInfo> descriptors;
	
	for (const auto& image : images) {
		VkDescriptorImageInfo currentImageDescriptor;
		currentImageDescriptor.imageLayout = std::get<0>(image);

		const auto currentImgView = std::get<1>(image);
		if (currentImgView)
			currentImageDescriptor.imageView = currentImgView->getNativeImageViewHandle();

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

	vkUpdateDescriptorSets(getParentDescriptorPool()->getParentDevice()->getNativeDeviceHandle(), 1, &writeDescriptorSet, 0, NULL);
}
