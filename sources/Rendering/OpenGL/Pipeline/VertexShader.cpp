#include "Rendering/OpenGL/Pipeline/VertexShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

VertexShader::VertexShader(const std::string& src) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), src) {}

VertexShader::VertexShader(const char* src, size_t srcSize) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), src, srcSize) {}