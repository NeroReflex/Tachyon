#pragma once

#include "DeviceOwned.h"
#include "SpaceRequiringResource.h"
#include "Memory/UnsafePoolManager.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class SpaceRequiringResource;

				class MemoryPool :
					virtual public DeviceOwned {
				
				public:
					MemoryPool(Device* device, VkMemoryPropertyFlagBits props, uint32_t memoryTypeBits, VkDeviceSize pagesCount, VkDeviceMemory&& memory) noexcept;

					MemoryPool(const MemoryPool&) = delete;

					MemoryPool(MemoryPool&&) = delete;

					MemoryPool& operator=(const MemoryPool&) = delete;
					
					VkDeviceSize malloc(const SpaceRequiringResource& resource, VkDeviceSize hint = 0) noexcept;

					void free(VkDeviceSize ptr, const SpaceRequiringResource& resource) noexcept;

					~MemoryPool() override;

					const VkMemoryPropertyFlagBits& getMemoryProperties() const noexcept;
					
					const VkDeviceMemory& getNativeDeviceMemoryHandle() const noexcept;

					/**
					 * This function is used to calculate the minimum number of atomic memory blocks required to safely store
					 * an object with the given size and alignment.
					 *
					 * Note: if you need to allocate on the same buffer more than one object just use the third parameter with previous results.
					 * Warning: if you have more than one resource to allocate you MUST NOT sum all size and alignments!
					 *
					 * @param size the buffer size (in bytes)
					 * @param alignment the alignment (in bytes)
					 * @param prev the result of a previous invocation
					 * @ return the minimum number of atomic memory blocks to be used for the allocation of the given set of resources.
					 */
					static VkDeviceSize getAtomicMemoryBlockCount(const VkDeviceSize& size, const VkDeviceSize& alignment, const VkDeviceSize& prev = 0) noexcept;

				private:
					VkMemoryPropertyFlagBits mProperties;

					uint32_t mMemoryTypeBits;

					VkDeviceMemory mDeviceMemory;

					void* const mFixedPageTracker;

					Memory::UnsafePoolManager mPoolManager;
				};
			}
		}
	}
}
