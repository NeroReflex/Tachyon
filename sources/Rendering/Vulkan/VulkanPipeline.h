#pragma once

#include "Rendering/RenderingPipeline.h"

namespace Tachyon {
    namespace Rendering {
		namespace Vulkan {
			class VulkanPipeline :
				virtual public Rendering::RenderingPipeline {
			public:
				VulkanPipeline() noexcept;

			};
		}
    }
}
