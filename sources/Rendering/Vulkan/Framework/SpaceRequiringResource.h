#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Device;

				class SpaceRequiringResource {

				public:
					SpaceRequiringResource() noexcept;

					SpaceRequiringResource(const SpaceRequiringResource&) = delete;

					SpaceRequiringResource(SpaceRequiringResource&&) = delete;

					SpaceRequiringResource& operator=(const SpaceRequiringResource&) = delete;

					virtual ~SpaceRequiringResource();

					uint32_t getRequiredSpace() const noexcept;

					uint32_t getRequiredAlignment() const noexcept;

					uint32_t getRequiredMemoryTypes() const noexcept;
					
					void bindMemory(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset ) noexcept;

					void unbindMemory() noexcept;
					
				protected:
					virtual void malloc(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset ) const noexcept = 0;
					
					void setMemoryRequirements(VkMemoryRequirements&& memReq) noexcept;

				private:
					bool mAllocated;
					
					bool mMemoryRequirementsSet = false;

					VkMemoryRequirements mMemoryRequirements;
				};
					
					
			}
		}
	}
}
