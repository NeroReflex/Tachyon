#include "Rendering/OpenGL/Pipeline/VertexShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

VertexShader::VertexShader(SourceType srcType, const std::string& src) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), srcType, src) {}

VertexShader::VertexShader(SourceType srcType, const char* src, size_t srcSize) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), srcType, src, srcSize) {}