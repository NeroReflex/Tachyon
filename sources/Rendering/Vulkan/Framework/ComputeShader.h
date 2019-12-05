#pragma once

#include "Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class ComputeShader :
					public Shader {

				public:
					inline ComputeShader(Device* device, const ShaderLayoutBinding& bindings, const char* source, uint32_t size) noexcept
						: DeviceOwned(device), Shader(device, Shader::ShaderType::Compute, bindings, source, size) {};

					ComputeShader() = delete;

					ComputeShader(const Shader&) = delete;

					ComputeShader(ComputeShader&&) = delete;

					ComputeShader& operator=(const ComputeShader&) = delete;

					~ComputeShader() override = default;
				};
			}
		}
	}
}
