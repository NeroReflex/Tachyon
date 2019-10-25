#pragma once

#include "Rendering/Renderer.h"

#include "Rendering/OpenGL/Pipeline/Program.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {

			class OpenGLRenderer :
				virtual public Rendering::Renderer {

			public:
				OpenGLRenderer() noexcept;

				~OpenGLRenderer() override;
				
				void render(const Core::RenderContext& scene, const Renderer::ShaderAlgorithm& shadingAlgo) noexcept final;

			private:
				std::unique_ptr<Pipeline::Program> mRaytracer;

				std::unique_ptr<Pipeline::Program> mDisplayWriter;

				glm::uint32 mOutputWidth, mOutputHeight;

				GLuint mTLAS;

				/**
				 * This is the output texture of the raytraing.
				 * This texture is not ready to be rendered as it is in RGBA32F format and pixels solors can exceed 1.0,
				 * so it has to be tone mapped.
				 */
				GLuint mRaytracerOutputTexture;

				/**
				 * This VAO is used for the final result rendering process (the one involving tonemapping).
				 */
				GLuint mFinalVAO;

				/**
				 * This VBO holds two triangles that directly map to the screen
				 */
				std::array<GLuint, 2> mVerticesVBO;
			};
		}
	}
}