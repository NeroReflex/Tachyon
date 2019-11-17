#pragma once

#include "Rendering/RenderingPipeline.h"

#include "Rendering/OpenGL/Pipeline/Program.h"

namespace Tachyon {
	namespace Rendering {
		namespace OpenGL {

			class OpenGLPipeline final :
				virtual public Rendering::RenderingPipeline {

			public:
				OpenGLPipeline(const OpenGLPipeline&) = delete;

				OpenGLPipeline(OpenGLPipeline&&) = delete;

				OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

				~OpenGLPipeline() override;

				OpenGLPipeline(GLFWwindow* window) noexcept;

				void enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept final;
				
				

			protected:
				void onReset() noexcept final;

				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept final;

				void onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept final;

			private:
				void update() noexcept;

			private:
				std::unique_ptr<Pipeline::Program> mRaytracerFlush;

				std::unique_ptr<Pipeline::Program> mRaytracerInsert;

				std::unique_ptr<Pipeline::Program> mRaytracerUpdate;

				std::unique_ptr<Pipeline::Program> mRaytracerRender;

				std::unique_ptr<Pipeline::Program> mDisplayWriter;

				GLuint mHDRBuffer;

				/**
				 * This is the mHDRBuffer memory mapped on the CPU.
				 */
				Core::HDR* mHDRMappedBuffer;

				/**
				 * Used to specify the camera to the render shader.
				 */
				GLuint mCameraUniformBuffer;

				GLuint mRaytracingTLAS;

				GLuint mRaytracingBLASCollection;

				GLuint mRaytracingGeometryCollection;

				GLuint mRaytracingModelMatrix;

				/**
				 * This is the output texture of the raytraing.
				 * This texture is not ready to be rendered as it is in RGBA32F format and pixels solors can exceed 1.0,
				 * so it has to be tone mapped.
				 */
				GLuint mRaytracerOutputTexture;

				/**
				 * A texture used for debugging. Has the screen dimensions.
				 */
				GLuint mRaytracerDebugOutput;

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