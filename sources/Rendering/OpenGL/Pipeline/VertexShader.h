#pragma once

#include "Rendering/OpenGL/Pipeline/Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {
			namespace Pipeline {
				class VertexShader final :
					public Shader {
				public:
					VertexShader() = delete;
					VertexShader(const VertexShader&) = delete;
					VertexShader(VertexShader&&) = delete;
					VertexShader& operator=(const VertexShader&) = delete;
					~VertexShader() final = default;

					VertexShader(SourceType srcType, const char* src, size_t srcSize, const std::string& entry = "main") noexcept;

					VertexShader(SourceType srcType, const std::string& src, const std::string& entry = "main") noexcept;
				};
			}
		}
	}
}