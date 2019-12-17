#include "MemoryPool.h"
#include "Device.h"
#include "SpaceRequiringResource.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

MemoryPool::MemoryPool(Device* device, VkMemoryPropertyFlagBits props, uint32_t memoryTypeBits, VkDeviceSize pagesCount, VkDeviceMemory&& memory) noexcept
	: DeviceOwned(device),
	mProperties(std::move(props)),
	mMemoryTypeBits(memoryTypeBits),
	mTotalSize(pagesCount * Memory::atomicMemoryBlockSize),
	mDeviceMemory(std::move(memory)),
	mFixedPageTracker(::malloc(Memory::UnsafePoolManager::getManagementReservedSpace(pagesCount))),
	mPoolManager(pagesCount, NULL, mFixedPageTracker),
	mMappedMemory(nullptr) {}

MemoryPool::~MemoryPool() {
	::free(mFixedPageTracker);

	vkFreeMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, nullptr);
}

const VkMemoryPropertyFlagBits& MemoryPool::getMemoryProperties() const noexcept {
	return mProperties;
}

const VkDeviceMemory& MemoryPool::getNativeDeviceMemoryHandle() const noexcept {
	return mDeviceMemory;
}

VkDeviceSize MemoryPool::malloc(const SpaceRequiringResource& resource, VkDeviceSize hint) noexcept {
	const auto requirements = resource.getMemoryRequirements();

	const auto result = mPoolManager.malloc(requirements.size, requirements.alignment);
	DBG_ASSERT(result.success);
	return uintptr_t(result.result);
}

void MemoryPool::free(VkDeviceSize ptr, const SpaceRequiringResource& resource) noexcept {
	const auto requirements = resource.getMemoryRequirements();
	mPoolManager.free((void*)(uintptr_t(ptr)), requirements.size, requirements.alignment);
}

VkDeviceSize MemoryPool::getAtomicMemoryBlockCount(const VkDeviceSize& size, const VkDeviceSize& alignment, const VkDeviceSize& prev) noexcept {
	return prev + 1 + ((alignment / Memory::atomicMemoryBlockSize) + (((alignment % Memory::atomicMemoryBlockSize) == 0) ? 0 : 1)) + ((size / Memory::atomicMemoryBlockSize) + (((size % Memory::atomicMemoryBlockSize) == 0) ? 0 : 1));
}

void* MemoryPool::mapMemory(VkDeviceSize offset, VkDeviceSize size) noexcept {
	DBG_ASSERT( (mProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) );
	DBG_ASSERT( ((offset + size) <= mTotalSize) );

	DBG_ASSERT( (mMappedMemory == nullptr) );

	void** ppPointer = &mMappedMemory;

	VK_CHECK_RESULT(vkMapMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory, offset, size, 0, ppPointer));

	return *ppPointer;
}

void MemoryPool::unmapMemory() noexcept {
	DBG_ASSERT((mMappedMemory != nullptr));

	vkUnmapMemory(getParentDevice()->getNativeDeviceHandle(), mDeviceMemory);

	mMappedMemory = nullptr;
}
