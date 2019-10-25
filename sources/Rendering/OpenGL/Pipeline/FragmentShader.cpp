#include "Rendering/OpenGL/Pipeline/FragmentShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

FragmentShader::FragmentShader(const std::string& src) noexcept
        : Shader(glCreateShader(GL_FRAGMENT_SHADER), src) {}

FragmentShader::FragmentShader(const char* src, size_t srcSize) noexcept
        : Shader(glCreateShader(GL_FRAGMENT_SHADER), src, srcSize) {}