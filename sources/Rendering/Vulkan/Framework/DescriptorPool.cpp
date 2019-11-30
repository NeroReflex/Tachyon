#include "DescriptorPool.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DescriptorPool::DescriptorPool(const Device* const device, VkDescriptorPool&& descriptorPool) noexcept
	: DeviceOwned(device),
	mDescriptorPool(std::move(descriptorPool)) {}

DescriptorPool::~DescriptorPool() {
	while (!mAllocatedDescriptorSets.empty()) {
		auto allocatedDescritorSet = mAllocatedDescriptorSets.begin();
		allocatedDescritorSet->second.reset(nullptr);
	}
	mAllocatedDescriptorSets.clear();

	vkDestroyDescriptorPool(getParentDevice()->getNativeDeviceHandle(), mDescriptorPool, nullptr);
}

const VkDescriptorPool& DescriptorPool::getNativeDescriptorPoolHandle() const noexcept {
	return mDescriptorPool;
}

DescriptorSet* DescriptorPool::allocateDescriptorSet(const Pipeline* pipeline) noexcept {
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &(pipeline->getNativeDescriptorSetLayout());

	VkDescriptorSet descriptorSet;
	VK_CHECK_RESULT(vkAllocateDescriptorSets(getParentDevice()->getNativeDeviceHandle(), &descriptorSetAllocateInfo, &descriptorSet));

	return new DescriptorSet(this, std::move(descriptorSet));
}