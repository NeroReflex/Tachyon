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

				void enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept override;
				
				void reset() noexcept override;

			protected:
				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

				void onRender() noexcept final;

			private:
				void insert(GLuint targetBLAS) noexcept;

				void flush() noexcept;

				void update() noexcept;

			private:
				std::unique_ptr<Pipeline::Program> mRaytracerQueryInfo;

				std::unique_ptr<Pipeline::Program> mRaytracerFlush;

				std::unique_ptr<Pipeline::Program> mRaytracerInsert;

				std::unique_ptr<Pipeline::Program> mRaytracerUpdate;

				std::unique_ptr<Pipeline::Program> mRaytracerRender;

				std::unique_ptr<Pipeline::Program> mDisplayWriter;

				struct RaytracerInfo {
					glm::uint32 expOfTwo_numberOfModels;
					glm::uint32 expOfTwo_numberOfGeometryCollectionOnBLAS;
					glm::uint32 expOfTwo_numberOfGeometryOnCollection;

					// Geometry is stored as consecutive vec4 in a texture, on the x axis.
					glm::uint32 oxpOfTwo_numberOfTesselsForGeometryTexturazation; // This number is the number of texture texels used to store the texture
				} mRaytracerInfo ;

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