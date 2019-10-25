#include "Rendering/OpenGL/Pipeline/Shader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

Shader::Shader(GLuint shader, const std::string& src) noexcept
    : Shader(shader, src.c_str(), src.size()) {}

Shader::Shader(GLuint shader, const char* src, size_t srcSize) noexcept
    : shader(shader) {
	const auto size = static_cast<GLint>(srcSize);
	glShaderSource(shader, 1, &src, &size);

	// Compile the vertex shader
	glCompileShader(shader);

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
