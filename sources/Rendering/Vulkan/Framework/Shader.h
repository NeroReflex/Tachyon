#pragma once

#include "DeviceOwned.h"
#include "ShaderLayoutBinding.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Shader :
					virtual public DeviceOwned {

				public:
					enum class ShaderType {
						Vertex,
						Geometry,
						Fragment,
						Compute,
					};

					Shader(const Device* device, ShaderType type, const ShaderLayoutBinding& bindings, char* source, uint32_t size) noexcept;

					Shader(const Shader&) = delete;

					Shader(Shader&&) = delete;

					Shader& operator=(const Shader&) = delete;

					~Shader() override;

					const VkShaderModule& getNativeShaderModuleHandle() const noexcept;

					const std::vector<VkDescriptorSetLayoutBinding>& getNativeShaderBindings() const noexcept;

				private:
					ShaderType mShaderType;

					VkShaderModule mShaderModule;

					std::vector<VkDescriptorSetLayoutBinding> mSpecializedBindings;
				};
			}
		}
	}
}
