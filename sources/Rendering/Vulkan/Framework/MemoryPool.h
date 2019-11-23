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
					MemoryPool(const Device* device, const std::initializer_list<const SpaceRequiringResource*>& resources) noexcept;

					MemoryPool(const MemoryPool&) = delete;

					MemoryPool(MemoryPool&&) = delete;

					MemoryPool& operator=(const MemoryPool&) = delete;

					~MemoryPool() override;

				private:
					uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const noexcept;

					VkDeviceMemory mDeviceMemory;
				};
			}
		}
	}
}