#pragma once

#include "Rendering/OpenGL/Pipeline/Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {
			namespace Pipeline {
				class Program {

					// TODO: think about this...
					friend class OpenGL;

				public:
					Program() = delete;

					Program(const Program&) = delete;

					Program& operator&(const Program&) = delete;

					~Program();

					Program(const std::initializer_list<std::shared_ptr<const Shader>>& shaders) noexcept;

					static void use(const Program& program) noexcept;

					void setUniform(const std::string& name, const glm::float32& value) const noexcept;

					void setUniform(const std::string& name, const glm::float32& data1,
						const glm::float32& data2) const noexcept;

					void setUniform(const std::string& name, const glm::vec2& data) const noexcept;

					void setUniform(const std::string& name, const GLfloat& data1, const GLfloat& data2,
						const GLfloat& data3) const noexcept;

					void setUniform(const std::string& name, const glm::vec3& data) const noexcept;

					void
						setUniform(const std::string& name, const GLfloat& data1, const GLfloat& data2,
							const GLfloat& data3,
							const GLfloat& data4) const noexcept;

					void setUniform(const std::string& name, const glm::vec4& data) const noexcept;

					void setUniform(const std::string& name, const glm::mat4& data) const noexcept;

					void setUniform(const std::string& name, const glm::mat3& data) const noexcept;

					void setUniform(const std::string& name, const glm::uint& data) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::vec3>& data) const noexcept;

					void setUniform(const std::string& name, const glm::vec4* data, const glm::uint count) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::vec4>& data) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::mat4>& data) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::mat3>& data) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::mat2x4>& data) const noexcept;

					void setUniform(const std::string& name, const glm::mat2x4* data, const glm::uint count) const noexcept;

					void setUniform(const std::string& name, const std::vector<glm::uint>& data) const noexcept;

				private:
					GLint getUniformLocation(const std::string& name) const noexcept;

					mutable std::unordered_map<std::string, GLint> uniformLocations;
					
					GLuint program;
				};
			}
		}
	}
}