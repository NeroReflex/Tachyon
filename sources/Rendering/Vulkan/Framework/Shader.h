#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Shader :
					virtual public DeviceOwned {

				public:
					Shader(const Shader&) = delete;

					Shader(Shader&&) = delete;

					Shader& operator=(const Shader&) = delete;

					~Shader() override;

					const VkShaderModule& getNativeShaderModuleHandle() const noexcept;

				protected:
					Shader(const Device* device, char* source, uint32_t size) noexcept;

				private:
					VkShaderModule mShaderModule;

					//VkShaderStage mPipeline;
				};
			}
		}
	}
}
