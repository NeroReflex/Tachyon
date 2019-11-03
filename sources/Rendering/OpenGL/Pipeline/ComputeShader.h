#pragma once

#include "Rendering/OpenGL/Pipeline/Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {
			namespace Pipeline {
				class ComputeShader final :
					public Shader {
				public:
					ComputeShader() = delete;
					ComputeShader(const ComputeShader&) = delete;
					ComputeShader& operator=(const ComputeShader&) = delete;
					~ComputeShader() final = default;

					ComputeShader(SourceType srcType, const char* src, size_t srcSize, const std::string& entry = "main") noexcept;

					ComputeShader(SourceType srcType, const std::string& src, const std::string& entry = "main") noexcept;
				};
			}
		}
	}
}