#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

ComputeShader::ComputeShader(SourceType srcType, const std::string& src, const std::string& entry) noexcept
        : Shader(glCreateShader(GL_COMPUTE_SHADER), srcType, src, entry) {}

ComputeShader::ComputeShader(SourceType srcType, const char* src, size_t srcSize, const std::string& entry) noexcept
        : Shader(glCreateShader(GL_COMPUTE_SHADER), srcType, src, srcSize, entry) {}