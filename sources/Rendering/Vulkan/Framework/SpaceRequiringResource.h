#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Device;
				class MemoryPool;

				class SpaceRequiringResource {

				public:
					SpaceRequiringResource() noexcept;

					SpaceRequiringResource(const SpaceRequiringResource&) = delete;

					SpaceRequiringResource(SpaceRequiringResource&&) = delete;

					SpaceRequiringResource& operator=(const SpaceRequiringResource&) = delete;

					virtual ~SpaceRequiringResource();

					const VkMemoryRequirements& getMemoryRequirements() const noexcept;
					
					void execBinding(const Device* const device, MemoryPool* memoryPool, VkDeviceSize offset) noexcept;

					MemoryPool* getAllocationMemoryPool() noexcept;

					const VkDeviceSize& getAllocationOffset() const noexcept;

				protected:
					virtual std::unique_ptr<VkMemoryRequirements> queryMemoryRequirements() const noexcept = 0;

					virtual void bindMemory(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset) const noexcept = 0;

				private:
					mutable std::unique_ptr<VkMemoryRequirements> mMemoryRequirements;

					MemoryPool* mUsedMemoryPool;

					VkDeviceSize mStartingOffset;
				};
					
					
			}
		}
	}
}
