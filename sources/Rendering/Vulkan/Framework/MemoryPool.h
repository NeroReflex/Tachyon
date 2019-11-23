#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class SpaceRequiringResource;

				class MemoryPool :
					virtual public DeviceOwned {

				public:
					MemoryPool(const Device* device, VkDeviceSize totalSize, VkDeviceMemory&& memory) noexcept;

					MemoryPool(const MemoryPool&) = delete;

					MemoryPool(MemoryPool&&) = delete;

					MemoryPool& operator=(const MemoryPool&) = delete;

					~MemoryPool() override;

				private:
					VkDeviceSize mTotalSize;

					VkDeviceMemory mDeviceMemory;
				};
			}
		}
	}
}