#include "Rendering/OpenGL/Pipeline/Shader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

Shader::Shader(GLuint shader, SourceType srcType, const std::string& src, const std::string& entry) noexcept
    : Shader(shader, srcType, src.c_str(), src.size(), entry) {}

Shader::Shader(GLuint shader, SourceType srcType, const char* src, size_t srcSize, const std::string& entry) noexcept
    : shader(shader) {
	const auto size = static_cast<GLint>(srcSize);

	if (srcType == SourceType::GLSL) {
		// Set the shader source code
		glShaderSource(shader, 1, &src, &size);

		// Compile the vertex shader
		glCompileShader(shader);
	} else if (srcType == SourceType::SPIRV) {
		// Apply the vertex shader SPIR-V to the shader object.
		glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, src, size);

		// Specialize the shader
		glSpecializeShader(shader, (const GLchar*)entry.c_str(), 0, nullptr, nullptr);
	} else {
		DBG_ASSERT(false);
	}

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		// Convert to string
		//writeError(std::move(std::string(infoLog.data())));

		DBG_ONLY({
			printf("Error in shader compilation: %s\n\n", infoLog.data());
		})
	}

	DBG_ASSERT( (isCompiled == GL_TRUE) );
}

Shader::~Shader() {
    // The shader has to be destroyed
    glDeleteShader(shader);
}
