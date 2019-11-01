#pragma once

#include "Rendering/OpenGL/Pipeline/Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {
			namespace Pipeline {
				class FragmentShader final :
					public Shader {
				public:
					FragmentShader() = delete;
					FragmentShader(const FragmentShader&) = delete;
					FragmentShader& operator=(const FragmentShader&) = delete;
					~FragmentShader() final = default;

					FragmentShader(SourceType srcType, const char* src, size_t srcSize) noexcept;

					FragmentShader(SourceType srcType, const std::string& src) noexcept;
				};
			}
		}
	}
}