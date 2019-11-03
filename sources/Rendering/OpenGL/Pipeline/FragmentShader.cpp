#include "Rendering/OpenGL/Pipeline/FragmentShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

FragmentShader::FragmentShader(SourceType srcType, const std::string& src, const std::string& entry) noexcept
        : Shader(glCreateShader(GL_FRAGMENT_SHADER), srcType, src, entry) {}

FragmentShader::FragmentShader(SourceType srcType, const char* src, size_t srcSize, const std::string& entry) noexcept
        : Shader(glCreateShader(GL_FRAGMENT_SHADER), srcType, src, srcSize, entry) {}