#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class SpaceRequiringResource {

				public:
					SpaceRequiringResource() = default;

					SpaceRequiringResource(const SpaceRequiringResource&) = delete;

					SpaceRequiringResource(SpaceRequiringResource&&) = delete;

					SpaceRequiringResource& operator=(const SpaceRequiringResource&) = delete;

					virtual ~SpaceRequiringResource() = default;

					const uint32_t& getRequiredSpace() const noexcept;

					const uint32_t& getRequiredAlignment() const noexcept;

					const uint32_t& getRequiredMemoryTypes() const noexcept;

				protected:
					void setMemoryRequirements(VkMemoryRequirements&& memReq) noexcept;

				private:
					bool mMemoryRequirementsSet = false;

					VkMemoryRequirements mMemoryRequirements;
				};
					
					
			}
		}
	}
}