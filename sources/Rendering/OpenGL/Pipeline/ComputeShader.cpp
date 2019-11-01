#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

ComputeShader::ComputeShader(SourceType srcType, const std::string& src) noexcept
        : Shader(glCreateShader(GL_COMPUTE_SHADER), srcType, src) {}

ComputeShader::ComputeShader(SourceType srcType, const char* src, size_t srcSize) noexcept
        : Shader(glCreateShader(GL_COMPUTE_SHADER), srcType, src, srcSize) {}