#include "Rendering/OpenGL/Pipeline/Program.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

Program::Program(const std::initializer_list<std::shared_ptr<const Shader>>& shaders) noexcept :
    program(glCreateProgram()) {
	// Attach each shader one after the other
	for (auto& shader : shaders) {
		//importErrors(std::string("Error in imported shader: "), *shader);

		glAttachShader(program, shader->shader);
	}

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)& isLinked);

	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// Register the error
		//writeError(std::move(std::string(infoLog.data())));
	}

	// Detach each shader one after the other
	for (auto& shader : shaders)
		glDetachShader(program, shader->shader);

    DBG_ASSERT( (isLinked == GL_FALSE) );
}

Program::~Program() {
    if (program != 0) {
        // Get the currently active program
        GLint64 activeProgram;
        glGetInteger64v(GL_CURRENT_PROGRAM,&activeProgram);

        if (program == activeProgram)
            glUseProgram(0);
    }

	glDeleteProgram(program);
}

void Program::use(const Program& program) noexcept {
    GLint64 activeProgram;
    glGetInteger64v(GL_CURRENT_PROGRAM, &activeProgram);

    if (program.program != activeProgram)
        glUseProgram(program.program);
}

GLint Program::getUniformLocation(const std::string& name) const noexcept {
    Program::use(*this);

    const auto locationIt = uniformLocations.find(name);
    if (locationIt == uniformLocations.cend()) {
        const auto location = glGetUniformLocation(program, reinterpret_cast<const GLchar *>(name.c_str()));
        uniformLocations[name] = location;
        return location;
    }

    return locationIt->second;
}

void Program::setUniform(const std::string& name, const glm::float32& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform1f(location, data);
}

void Program::setUniform(const std::string& name, const glm::float32& data1, const glm::float32& data2) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform2f(location, data1, data2);
}

void Program::setUniform(const std::string& name, const glm::vec2& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform2f(location, data[0], data[1]);
}

void Program::setUniform(const std::string& name, const GLfloat& data1, const GLfloat& data2, const GLfloat& data3) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform3f(location, data1, data2, data3);
}

void Program::setUniform(const std::string& name, const glm::vec3& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform3f(location, data[0], data[1], data[2]);
}

void Program::setUniform(const std::string& name, const GLfloat& data1, const GLfloat& data2, const GLfloat& data3, const GLfloat& data4) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform4f(location, data1, data2, data3, data4);
}

void Program::setUniform(const std::string& name, const glm::vec4& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform4f(location, data[0], data[1], data[2], data[3]);
}

void Program::setUniform(const std::string& name, const glm::mat4& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
}

void Program::setUniform(const std::string& name, const glm::mat3& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(data));
}

void Program::setUniform(const std::string& name, const glm::uint& data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform1ui(location, data);
}


void Program::setUniform(const std::string &name, const std::vector<glm::vec3> &data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform3fv(location, data.size(), glm::value_ptr(*(data.data())));
}

void Program::setUniform(const std::string &name, const std::vector<glm::vec4> &data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform4fv(location, data.size(), glm::value_ptr(*(data.data())));
}

void Program::setUniform(const std::string &name, const std::vector<glm::mat4> &data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniformMatrix4fv(location, data.size(), GL_FALSE, glm::value_ptr(*(data.data())));
}

void Program::setUniform(const std::string& name, const std::vector<glm::mat2x4>& data) const noexcept {
	const auto location = getUniformLocation(name);
	if (location >= 0)
		glUniformMatrix2x4fv(location, data.size(), GL_FALSE, glm::value_ptr(*(data.data())));
}

void Program::setUniform(const std::string &name, const std::vector<glm::mat3> &data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniformMatrix3fv(location, data.size(), GL_FALSE, glm::value_ptr(*(data.data())));
}

void Program::setUniform(const std::string &name, const std::vector<glm::uint> &data) const noexcept {
    const auto location = getUniformLocation(name);
    if (location >= 0)
        glUniform1uiv(location, data.size(), data.data());
}

void Program::setUniform(const std::string& name, const glm::vec4* data, const glm::uint count) const noexcept {
	const auto location = getUniformLocation(name);
	if (location >= 0)
		glUniform4fv(location, count, glm::value_ptr(*data));
}

void Program::setUniform(const std::string& name, const glm::mat2x4* data, glm::uint count) const noexcept {
	const auto location = getUniformLocation(name);
	if (location >= 0)
		glUniformMatrix2x4fv(location, count, GL_FALSE, glm::value_ptr(*data));
}
