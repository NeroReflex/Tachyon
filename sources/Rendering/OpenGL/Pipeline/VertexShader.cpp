#include "Rendering/OpenGL/Pipeline/VertexShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

VertexShader::VertexShader(SourceType srcType, const std::string& src, const std::string& entry) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), srcType, src, entry) {}

VertexShader::VertexShader(SourceType srcType, const char* src, size_t srcSize, const std::string& entry) noexcept
    : Shader(glCreateShader(GL_VERTEX_SHADER), srcType, src, srcSize, entry) {}