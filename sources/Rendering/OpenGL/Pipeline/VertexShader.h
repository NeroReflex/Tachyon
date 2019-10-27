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

					VertexShader(const char* src, size_t srcSize) noexcept;

					VertexShader(const std::string& src) noexcept;
				};
			}
		}
	}
}