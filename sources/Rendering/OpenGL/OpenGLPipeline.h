#pragma once

#include "Rendering/RenderingPipeline.h"

#include "Rendering/OpenGL/Pipeline/Program.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {

			class OpenGLPipeline :
				virtual public Rendering::RenderingPipeline {

			public:
				OpenGLPipeline(const OpenGLPipeline&) = delete;

				OpenGLPipeline(OpenGLPipeline&&) = delete;

				OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

				~OpenGLPipeline() override;

				OpenGLPipeline() noexcept;

				void enqueueModel(const std::vector<GeometryPrimitive>& primitive, GLuint location) noexcept override;
				
			protected:
				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

				void onRender() noexcept final;

			private:
				void prapareDispatch() noexcept;

				void insert(GLuint targetBLAS) noexcept;

				void flush() noexcept;

				void update() noexcept;

			private:
				std::unique_ptr<Pipeline::Program> mRaytracerFlush;

				std::unique_ptr<Pipeline::Program> mRaytracerInsert;

				std::unique_ptr<Pipeline::Program> mRaytracerUpdate;

				std::unique_ptr<Pipeline::Program> mRaytracerRender;

				std::unique_ptr<Pipeline::Program> mDisplayWriter;

				/**
				 * This is the input data for the GPU raytracer algorithm.
				 * See raytracer compute shader for more details.
				 */
				std::array<GLuint, 3> mRaytracingSSBO;

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

				/**
				 * This is an SSBO, mapped in GPU Shared memory used to create scene object
				 */
				GLuint mInputGeometryTemporary;
			};
		}
	}
}