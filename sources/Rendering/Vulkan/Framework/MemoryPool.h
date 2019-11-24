#pragma once

#include "DeviceOwned.h"
#include "Memory/PoolManager.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class SpaceRequiringResource;

				class MemoryPool :
					virtual public DeviceOwned {
				
				public:
					MemoryPool(const Device* device, VkMemoryPropertyFlagBits props, uint32_t memoryTypeBits, VkDeviceSize pagesCount, VkDeviceMemory&& memory) noexcept;

					MemoryPool(const MemoryPool&) = delete;

					MemoryPool(MemoryPool&&) = delete;

					MemoryPool& operator=(const MemoryPool&) = delete;

					~MemoryPool() override;

					const VkMemoryPropertyFlagBits& getMemoryProperties() const noexcept;

				private:
					VkMemoryPropertyFlagBits mProperties;

					uint32_t mMemoryTypeBits;

					VkDeviceMemory mDeviceMemory;

					uint32_t* const mFixedPageTracker;

					Memory::PoolManager mPoolManager;
				};
			}
		}
	}
}