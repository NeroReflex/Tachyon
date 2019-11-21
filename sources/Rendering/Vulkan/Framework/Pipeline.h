#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Pipeline :
					virtual public DeviceOwned {

				public:
					Pipeline(const Device* device, VkPipeline&& pipeline) noexcept;

					Pipeline(const Pipeline&) = delete;

					Pipeline(Pipeline&&) = delete;

					Pipeline& operator=(const Pipeline&) = delete;

					~Pipeline() override;

					const VkPipeline& getNativePipelineHandle() const noexcept;

				private:
					VkPipeline mPipeline;
				};
			}
		}
	}
}