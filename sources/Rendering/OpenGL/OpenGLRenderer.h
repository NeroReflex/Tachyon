#pragma once

#include "Rendering/Renderer.h"

#include "Rendering/OpenGL/Pipeline/Program.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {

			class OpenGLRenderer :
				virtual public Rendering::Renderer {

			public:
				OpenGLRenderer(const OpenGLRenderer&) = delete;

				OpenGLRenderer(OpenGLRenderer&&) = delete;

				OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;

				~OpenGLRenderer() override;

				OpenGLRenderer(const Core::RenderContext& scene, glm::uint32 width, glm::uint32 height) noexcept;
				
				void render(const Renderer::ShaderAlgorithm& shadingAlgo) noexcept final;

			protected:
				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept override;

			private:
				std::unique_ptr<Pipeline::Program> mRaytracer;

				std::unique_ptr<Pipeline::Program> mDisplayWriter;

				/**
				 * This is the output texture of the raytraing.
				 * This texture is not ready to be rendered as it is in RGBA32F format and pixels solors can exceed 1.0,
				 * so it has to be tone mapped.
				 */
				GLuint mRaytracerOutputTexture;

				/**
				 * This VAO is used for the final result rendering process (the one involving tonemapping).
				 */
				GLuint mQuadVAO;

				/**
				 * This VBO holds two triangles that directly map to the screen
				 */
				GLuint mQuadVBO;
			};
		}
	}
}