#include "Rendering/OpenGL/OpenGLPipeline.h"

#include "Rendering/OpenGL/Pipeline/VertexShader.h"
#include "Rendering/OpenGL/Pipeline/FragmentShader.h"
#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

#include "shaders/config.glsl"
#include "shaders/opengl/tonemapping.vert.spv.h" // SHADER_TONEMAPPING_VERT, SHADER_TONEMAPPING_VERT_size
#include "shaders/opengl/tonemapping.frag.spv.h" // SHADER_TONEMAPPING_FRAG, SHADER_TONEMAPPING_FRAG_size
#include "shaders/opengl/raytrace_insert.comp.spv.h" // raytrace_insert_compOGL, raytrace_insert_compOGL_size
#include "shaders/opengl/raytrace_flush.comp.spv.h" // raytrace_flush_compOGL, raytrace_flush_compOGL_size
#include "shaders/opengl/raytrace_render.comp.spv.h" // raytrace_render_compOGL, raytrace_render_compOGL_size
#include "shaders/opengl/raytrace_update.comp.spv.h" // raytrace_update_compOGL, raytrace_update_compOGL_size

OpenGLPipeline::OpenGLPipeline(GLFWwindow* window) noexcept
    : RenderingPipeline(window),
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
	mRaytracerDebugOutput(0),
	mRaytracingTLAS(0) {

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

	// Create the HDR buffer
	glCreateBuffers(1, &mHDRBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHDRBuffer);
	glNamedBufferStorage(mHDRBuffer, sizeof(Core::HDR), NULL, GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
	mHDRMappedBuffer = reinterpret_cast<Core::HDR*>(glMapNamedBuffer(mHDRBuffer, GL_WRITE_ONLY));
	DBG_ASSERT((mHDRMappedBuffer != nullptr));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, HDR_BINDING, mHDRBuffer);

	// Finalize VBOs
	glCreateBuffers(1, &mQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glNamedBufferStorage(mQuadVBO, sizeof(glm::vec4) * screenTrianglesPosition.size(), screenTrianglesPosition.data(), 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexArrayAttrib(mQuadVAO, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

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
	glTextureStorage1D(mRaytracingTLAS, 1, GL_RGBA32F, mRaytracerRequirements.mTLASTexels_Width);
	// END OF TLAS TEXTURE CREATION

	// BLAS COLLECTION TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracingBLASCollection);
	glBindTexture(GL_TEXTURE_2D, mRaytracingBLASCollection);
	glTextureStorage2D(mRaytracingBLASCollection, 1, GL_RGBA32F, mRaytracerRequirements.mBLASCollectionTexels_Width, mRaytracerRequirements.mBLASCollectionTexels_Height);
	// END OF BLAS COLLECTION TEXTURE CREATION

	// MODELMATRIX TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracingModelMatrix);
	glBindTexture(GL_TEXTURE_2D, mRaytracingModelMatrix);
	glTextureStorage2D(mRaytracingModelMatrix, 1, GL_RGBA32F, 4, size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels);
	// END OF MODELMATRIX TEXTURE CREATION

	// GEOMETRY COLLECTION TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_3D, 1, &mRaytracingGeometryCollection);
	glBindTexture(GL_TEXTURE_3D, mRaytracingGeometryCollection);
	glTextureStorage3D(mRaytracingGeometryCollection, 1, GL_RGBA32F, mRaytracerRequirements.mGeometryCollectionTexels_Width, mRaytracerRequirements.mGeometryCollectionTexels_Height, mRaytracerRequirements.mGeometryCollectionTexels_Depth);
	// END OF GEOMETRY COLLECTION TEXTURE CREATION

	glViewport(0, 0, getWidth(), getHeight());

	// OUTPUT TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerOutputTexture);
	glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
	glTextureStorage2D(mRaytracerOutputTexture, 1, GL_RGBA32F, getWidth(), getHeight());
	// END OF OUTPUT TEXTURE CREATION

	// DEBUG TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerDebugOutput);
	glBindTexture(GL_TEXTURE_2D, mRaytracerDebugOutput);
	glTextureStorage2D(mRaytracerDebugOutput, 1, GL_RGBA32F, getWidth(), getHeight());
	glBindImageTexture(DEBUG_BINDING, mRaytracerDebugOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	// END OF DEBUG TEXTURE CREATION
	
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

	// Remove the HDR buffer
	glUnmapBuffer(mHDRBuffer);

	// Remove main VAO
	glDeleteVertexArrays(1, &mQuadVAO);

	// Remove VBO
	glDeleteBuffers(1, &mQuadVBO);
}

void OpenGLPipeline::onReset() noexcept {
	Program::use(*mRaytracerFlush);

	// Bind the raytracer ModelMatrix as write-only as the shader will only nuke it
	glBindImageTexture(BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// The TLAS may get updated to the root after a leaf deletion
	glBindImageTexture(TLAS_BINDING, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels, 1, 1);

	// synchronize with the GPU
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void OpenGLPipeline::onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept {
	// Clear the previously rendered scene
	glClear(GL_COLOR_BUFFER_BIT);

	// Update the TLAS before rendering
	//update();

	// Set the raytracer program as the active one
	Program::use(*mRaytracerRender);

	// Bind the raytracer render context as read-only!
	glBindImageTexture(TLAS_BINDING, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(BLAS_BINDING, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(GEOMETRY_BINDING, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	// Bind the texture to be written by the raytracer
	glBindImageTexture(OUTPUT_BINDING, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Set rendering information
	mRaytracerRender->setUniform("width", getWidth());
	mRaytracerRender->setUniform("height", getHeight());

	// Set camera parameters
	mRaytracerRender->setUniform("cameraPosition", camera.getCameraPosition());
	mRaytracerRender->setUniform("cameraViewDir", camera.getViewDirection());
	mRaytracerRender->setUniform("cameraUpVector", camera.getUp());
	mRaytracerRender->setUniform("cameraFoV", camera.getFieldOfView());
	mRaytracerRender->setUniform("cameraAspect", camera.getAspectRatio());
	mRaytracerRender->setUniform("cameraLowerLeftCorner", camera.getRaytracingLowerLeftCorner());
	mRaytracerRender->setUniform("cameraHorizontal", camera.getRaytracingHorizontal());
	mRaytracerRender->setUniform("cameraVertical", camera.getRaytracingVertical());

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);
	glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Set HDR paremeters for rendering
	*mHDRMappedBuffer = hdr;

	mDisplayWriter->setUniform("gamma", glm::float32(2.2));
	mDisplayWriter->setUniform("exposure", glm::float32(0.1));

	// Bind the texture generated by raytracing
	glBindTextureUnit(OUTPUT_BINDING, mRaytracerOutputTexture);

	// make sure writing to render target has finished before using it
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

	// Draw the generated image while gamma-correcting it
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OpenGLPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {
	glViewport(0, 0, newWidth, newHeight);

	// Create a new 2D texture used to store the raw raytrace result (without gamma correction)
	glDeleteTextures(1, &mRaytracerOutputTexture); // Remove the previous texture to avoid GPU memory leak(s)
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerOutputTexture);
	glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
	glTextureStorage2D(mRaytracerOutputTexture, 1, GL_RGBA32F, newWidth, newHeight);

	// debug output
	glDeleteTextures(1, &mRaytracerDebugOutput);
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerDebugOutput);
	glBindTexture(GL_TEXTURE_2D, mRaytracerDebugOutput);
	glTextureStorage2D(mRaytracerDebugOutput, 1, GL_RGBA32F, newWidth, newHeight);
	glBindImageTexture(DEBUG_BINDING, mRaytracerDebugOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

}

void OpenGLPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitivesCollection, GLuint targetBLAS) noexcept {
	DBG_ASSERT( (targetBLAS < (static_cast<GLuint>(1) << mRaytracerInfo.expOfTwo_numberOfModels)) );

	static_assert( (sizeof(GeometryPrimitive) == sizeof(glm::vec4) ), "Geometry type not matching input GLSL");
	
	// When creating the SSBO used to write geometry on the GPU the primitivesCollection vector will be read sequentially for the entire SSBO length, so make sure that won't generate a SEGFAULT or incorrect AABBs...
	while (primitivesCollection.size() < ((size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS))) {
		// .... by postponing empty geometry
		primitivesCollection.emplace_back(GeometryPrimitive());
	}

	// Prepare the tomporary input geometry SSBO
	GLuint temporaryInputGeometry;
	glCreateBuffers(1, &temporaryInputGeometry);
	glNamedBufferStorage(temporaryInputGeometry, sizeof(glm::vec4) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS) * size_t(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection), primitivesCollection.data(), 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, temporaryInputGeometry);

	Program::use(*mRaytracerInsert);

	mRaytracerInsert->setUniform("targetBLAS", targetBLAS);

	glBindImageTexture(TLAS_BINDING, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(BLAS_BINDING, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(GEOMETRY_BINDING, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	
	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection, size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS, 1);

	// synchronize with the GPU: the insert procedure writes to texture (BLAS) and to the ModelMatrix SSBO.
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glDeleteBuffers(1, &temporaryInputGeometry);
}

void OpenGLPipeline::update() noexcept {
	Program::use(*mRaytracerUpdate);

	glBindImageTexture(TLAS_BINDING, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(BLAS_BINDING, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(GEOMETRY_BINDING, mRaytracingGeometryCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels, 1, 1);

	// synchronize with the GPU: the update procedure only write to texture (TLAS)
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
