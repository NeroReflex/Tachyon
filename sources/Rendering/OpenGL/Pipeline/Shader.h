#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
			namespace OpenGL {
				namespace Pipeline {
				class Program;

				class Shader {
					friend class Program;

				public:
					Shader() = delete;
					Shader(const Shader&) = delete;
					Shader(Shader&&) = delete;
					Shader& operator=(const Shader&) = delete;

					virtual ~Shader();

				protected:
					Shader(GLuint shader, const char* src, size_t srcSize) noexcept;

					Shader(GLuint shader, const std::string& src) noexcept;

				private:
					GLuint shader;
				};
			}
		}
	}
}