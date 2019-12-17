#include "Buffer.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Buffer::Buffer(Device* const device, VkBufferUsageFlagBits usage, VkDeviceSize size, VkBuffer&& buffer) noexcept
	: DeviceOwned(device),
	mUsage(std::move(usage)),
	mSize(size),
	mBuffer(std::move(buffer)) {}

Buffer::~Buffer() {
	vkDestroyBuffer(getParentDevice()->getNativeDeviceHandle(), mBuffer, nullptr);
}

const VkBuffer& Buffer::getNativeBufferHandle() const noexcept {
	return mBuffer;
}

std::unique_ptr<VkMemoryRequirements> Buffer::queryMemoryRequirements() const noexcept {
	VkMemoryRequirements* memRequirements = new VkMemoryRequirements();
	vkGetBufferMemoryRequirements(getParentDevice()->getNativeDeviceHandle(), getNativeBufferHandle(), memRequirements);
	
	return std::move(std::unique_ptr<VkMemoryRequirements>(memRequirements));
}

void Buffer::bindMemory(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset) const noexcept {
	VK_CHECK_RESULT(vkBindBufferMemory(device->getNativeDeviceHandle(), getNativeBufferHandle(), memoryPool, offset));
}

const VkBufferUsageFlagBits& Buffer::getBufferUsage() const noexcept {
	return mUsage;
}

const VkDeviceSize& Buffer::getBufferSize() const noexcept {
	return mSize;
}
