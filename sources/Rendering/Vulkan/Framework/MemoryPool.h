#pragma once

#include "DeviceOwned.h"
#include "SpaceRequiringResource.h"
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
					
					VkDeviceSize malloc(const SpaceRequiringResource& resource, VkDeviceSize hint = 0) noexcept;

					void free(VkDeviceSize ptr, const SpaceRequiringResource& resource) noexcept;

					~MemoryPool() override;

					const VkMemoryPropertyFlagBits& getMemoryProperties() const noexcept;
					
					const VkDeviceMemory& getNativeDeviceMemoryHandle() const noexcept;

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
