#include "Rendering/OpenGL/OpenGLPipeline.h"

#include "Rendering/OpenGL/Pipeline/VertexShader.h"
#include "Rendering/OpenGL/Pipeline/FragmentShader.h"
#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

#include "shaders/tonemapping.vert.spv.h" // SHADER_TONEMAPPING_VERT, SHADER_TONEMAPPING_VERT_size
#include "shaders/tonemapping.frag.spv.h" // SHADER_TONEMAPPING_FRAG, SHADER_TONEMAPPING_FRAG_size
#include "shaders/raytrace_insert.comp.spv.h" // raytrace_insert_compOGL, raytrace_insert_compOGL_size
#include "shaders/raytrace_flush.comp.spv.h" // raytrace_flush_compOGL, raytrace_flush_compOGL_size
#include "shaders/raytrace_render.comp.spv.h" // raytrace_render_compOGL, raytrace_render_compOGL_size
#include "shaders/raytrace_update.comp.spv.h" // raytrace_update_compOGL, raytrace_update_compOGL_size
#include "shaders/raytrace_query_info.comp.spv.h" // raytrace_query_info_compOGL raytrace_query_info_compOGL_size

OpenGLPipeline::OpenGLPipeline() noexcept
    : RenderingPipeline(),
	mRaytracerQueryInfo(
		new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_query_info_compOGL), raytrace_query_info_compOGL_size)
		})
	),
	mRaytracerFlush(
		new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_flush_compOGL), raytrace_flush_compOGL_size)
		})
	),
	mRaytracerUpdate(
		new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_update_compOGL), raytrace_update_compOGL_size)
		})
	),
	mRaytracerInsert(new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_insert_compOGL), raytrace_insert_compOGL_size)
		})
	),
	mRaytracerRender(new Pipeline::Program(
        std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char *>(raytrace_render_compOGL), raytrace_render_compOGL_size)
		})
	),
	mDisplayWriter(new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const VertexShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(tonemapping_vertOGL), tonemapping_vertOGL_size),
			std::make_shared<const FragmentShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(tonemapping_fragOGL), tonemapping_fragOGL_size)
		})
    ),
	mRaytracerOutputTexture(0),
	mRaytracingTLAS(0) {

	// Query raytracer capabilities
	GLuint mRaytracerInfoSSBO;
	glCreateBuffers(1, &mRaytracerInfoSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRaytracerInfoSSBO);
	glNamedBufferStorage(mRaytracerInfoSSBO, sizeof(RaytracerInfo), NULL, GL_MAP_READ_BIT); // when done I want to read back results
	Program::use(*mRaytracerQueryInfo); // This is the program that I use to query raytracer capabilities
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mRaytracerInfoSSBO); // Bind the buffer to be used to retrieve info about the raytracer
	glDispatchCompute(1, 1, 1); // Only one work is needed
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // Wit for the GPU to write out results
	const RaytracerInfo* infoPtr = reinterpret_cast<const RaytracerInfo*>(glMapNamedBufferRange(mRaytracerInfoSSBO, 0, sizeof(RaytracerInfo), GL_MAP_READ_BIT)); // map the memory so I can read the capabilities of the raytracer
	DBG_ASSERT( (infoPtr != nullptr) );
	mRaytracerInfo = *infoPtr; // Copy info retrieved from the GPU to a more conventional type of memory
	glUnmapNamedBuffer(mRaytracerInfoSSBO); // Done, unmap the memory
	glDeleteBuffers(1, &mRaytracerInfoSSBO); // Done, delete the GPU memory

	std::array<glm::vec4, 4> screenTrianglesPosition = {
		glm::vec4(-1.0f, -1.0f, 0.5f, 1.0f),
        glm::vec4(1.0f, -1.0f, 0.5f, 1.0f),
        glm::vec4(1.0f,  1.0f, 0.5f, 1.0f),
        glm::vec4(-1.0f,  1.0f, 0.5f, 1.0f),
	};
	
	// Set OpenGL clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Create the final VAO
	glCreateVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);

	// Create final VBOs
	glCreateBuffers(1, &mQuadVBO);

	// Finalize VBOs
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glNamedBufferStorage(mQuadVBO, sizeof(glm::vec4) * screenTrianglesPosition.size(), screenTrianglesPosition.data(), 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexArrayAttrib(mQuadVAO, 0);

	// Activate needed texture units
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glActiveTexture(GL_TEXTURE2);
	glActiveTexture(GL_TEXTURE3);
	glActiveTexture(GL_TEXTURE4);
	glActiveTexture(GL_TEXTURE5);

	// TLAS TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_1D, 1, &mRaytracingTLAS);
	glBindTexture(GL_TEXTURE_1D, mRaytracingTLAS);
	glTextureStorage1D(mRaytracingTLAS, 1, GL_RGBA32F, (size_t(1) << (mRaytracerInfo.expOfTwo_numberOfModels + 1)) * 2);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF TLAS TEXTURE CREATION

	// BLAS COLLECTION TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracingBLASCollection);
	glBindTexture(GL_TEXTURE_2D, mRaytracingBLASCollection);
	glTextureStorage2D(mRaytracingBLASCollection, 1, GL_RGBA32F, (size_t(1) << (mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS + 1)) * 2, size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF BLAS COLLECTION TEXTURE CREATION

	// MODELMATRIX TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracingModelMatrix);
	glBindTexture(GL_TEXTURE_2D, mRaytracingModelMatrix);
	glTextureStorage2D(mRaytracingModelMatrix, 1, GL_RGBA32F, 4, size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF MODELMATRIX TEXTURE CREATION

	// GEOMETRY COLLECTION TEXTURE CREATION
	size_t geometryX = size_t(1) << (mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection + mRaytracerInfo.oxpOfTwo_numberOfTesselsForGeometryTexturazation);
	size_t geometryY = size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS;
	size_t geometryZ = size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels;
	glCreateTextures(GL_TEXTURE_3D, 1, &mRaytracingGeometryCollection);
	glBindTexture(GL_TEXTURE_3D, mRaytracingGeometryCollection);
	glTextureStorage3D(
		mRaytracingGeometryCollection,
		1, GL_RGBA32F,
		size_t(1) << (mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection + mRaytracerInfo.oxpOfTwo_numberOfTesselsForGeometryTexturazation),
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS,
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF GEOMETRY COLLECTION TEXTURE CREATION
	
	// The VAO with the screen quad needs to be binded only once as the raytracing never uses any other VAOs
	glBindVertexArray(mQuadVAO);
}

OpenGLPipeline::~OpenGLPipeline() {
	// Delete textures used to store the scene
	glDeleteTextures(1, &mRaytracingTLAS);
	glDeleteTextures(1, &mRaytracingBLASCollection);
	glDeleteTextures(1, &mRaytracingGeometryCollection);
	glDeleteTextures(1, &mRaytracingModelMatrix);

	// Avoid removing a VAO while it is currently bound
	glBindVertexArray(0);

	// Remove main VAO
	glDeleteVertexArrays(1, &mQuadVAO);

	// Remove VBO
	glDeleteBuffers(1, &mQuadVBO);
}

void OpenGLPipeline::reset() noexcept {
	flush();
}

void OpenGLPipeline::onRender() noexcept {
	// Clear the previously rendered scene
	glClear(GL_COLOR_BUFFER_BIT);

	// Update the TLAS before rendering
	update();

	// Set the raytracer program as the active one
	Program::use(*mRaytracerRender);

	// Bind the raytracer render context as read-only!
	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(3, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	// Bind the texture to be written by the raytracer
	glBindImageTexture(5, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Set rendering information
	mRaytracerRender->setUniform("width", getWidth());
	mRaytracerRender->setUniform("height", getHeight());

	// Set camera parameters
	mRaytracerRender->setUniform("cameraPosition", glm::vec3(0, 0, 0));
	mRaytracerRender->setUniform("cameraViewDir", glm::vec3(0, 0, -1));
	mRaytracerRender->setUniform("cameraUpVector", glm::vec3(0, 1, 0));
	mRaytracerRender->setUniform("cameraFoV", glm::float32(60));
	mRaytracerRender->setUniform("cameraAspect", glm::float32(getWidth()) / glm::float32(getHeight()));

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);

	// Set parameters to obtain hdr
	mDisplayWriter->setUniform("gamma", glm::float32(2.2));
	mDisplayWriter->setUniform("exposure", glm::float32(0.1));

	// Bind the texture generated by raytracing
	glBindTextureUnit(5, mRaytracerOutputTexture);

	// Draw the generated image while gamma-correcting it
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OpenGLPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {
	glViewport(0, 0, newWidth, newHeight);

	// Remove the previous texture to avoid GPU memory leak(s)
	if (mRaytracerOutputTexture)
		glDeleteTextures(1, &mRaytracerOutputTexture);

	// Create a new 2D texture used to store the raw raytrace result (without gamma correction)
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerOutputTexture);
	glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
	glTextureStorage2D(mRaytracerOutputTexture, 1, GL_RGBA32F, newWidth, newHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
}

void OpenGLPipeline::flush() noexcept {
	Program::use(*mRaytracerFlush);

	// Bind the raytracer ModelMatrix as write-only as the shader will only nuke it
	glBindImageTexture(3, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// The TLAS may get updated to the root after a leaf deletion
	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels, 1, 1);

	// synchronize with the GPU
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void OpenGLPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitivesCollection, GLuint targetBLAS) noexcept {
	DBG_ASSERT( (targetBLAS < (1 << mRaytracerInfo.expOfTwo_numberOfModels)) );

	static_assert( (sizeof(GeometryPrimitive) == sizeof(glm::vec4) ), "Geometry type not matching input GLSL");
	
	// When creating the SSBO used to write geometry on the GPU the primitivesCollection vector will be read sequentially for the entire SSBO length, so make sure that won't generate a SEGFAULT!
	primitivesCollection.reserve((size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS));

	// Prepare the tomporary input geometry SSBO
	GLuint temporaryInputGeometry;
	glCreateBuffers(1, &temporaryInputGeometry);
	glNamedBufferStorage(temporaryInputGeometry, sizeof(glm::vec4) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS) * size_t(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection), primitivesCollection.data(), 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, temporaryInputGeometry);

	Program::use(*mRaytracerInsert);

	mRaytracerInsert->setUniform("targetBLAS", targetBLAS);

	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	
	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection, size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS, 1);

	// synchronize with the GPU: the insert procedure writes to texture (BLAS) and to the ModelMatrix SSBO.
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glDeleteBuffers(1, &temporaryInputGeometry);
}

void OpenGLPipeline::update() noexcept {
	Program::use(*mRaytracerUpdate);

	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(3, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels, 1, 1);

	// synchronize with the GPU: the update procedure only write to texture (TLAS)
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
