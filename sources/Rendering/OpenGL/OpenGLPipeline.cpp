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

	// Create the HDR uniform buffer
	glCreateBuffers(1, &mHDRUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mHDRUniformBuffer);
	glNamedBufferStorage(mHDRUniformBuffer, sizeof(Core::HDR), NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, HDR_BINDING, mHDRUniformBuffer);

	// Create the camera uniform buffer
	glCreateBuffers(1, &mCameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	glNamedBufferStorage(mCameraUniformBuffer, sizeof(Core::Camera), NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, mCameraUniformBuffer);

	// Create the insertion geometry attributes buffer
	glCreateBuffers(1, &mGeometryInsertAttributesUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mGeometryInsertAttributesUniformBuffer);
	glNamedBufferStorage(mGeometryInsertAttributesUniformBuffer, sizeof(glm::uint32) + sizeof(glm::mat4), NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, GEOMETRY_INSERTT_ATTR_BINDING, mGeometryInsertAttributesUniformBuffer);

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
	glActiveTexture(GL_TEXTURE6);
	glActiveTexture(GL_TEXTURE7);
	glActiveTexture(GL_TEXTURE8);
	glActiveTexture(GL_TEXTURE9);
	glActiveTexture(GL_TEXTURE10);

	// TLAS CREATION
	glCreateBuffers(1, &mRaytracingTLAS);
	glNamedBufferStorage(mRaytracingTLAS, (2 * 16) * (static_cast<uint32_t>(1) << numberOfTreeElementsToContainExpOfTwoLeafs(expOfTwo_maxModels)), NULL, 0);
	// END OF TLAS CREATION

	// BLAS COLLECTION CREATION
	glCreateBuffers(1, &mRaytracingBLASCollection);
	glNamedBufferStorage(mRaytracingBLASCollection, (2 * 16) * (static_cast<uint32_t>(1) << expOfTwo_maxModels) * numberOfTreeElementsToContainExpOfTwoLeafs(expOfTwo_maxCollectionsForModel), NULL, 0);
	// END OF BLAS COLLECTION CREATION

	// MODELMATRIX CREATION
	glCreateBuffers(1, &mRaytracingModelMatrix);
	glNamedBufferStorage(mRaytracingModelMatrix, sizeof(glm::mat4) * (static_cast<uint32_t>(1) << expOfTwo_maxModels), NULL, 0);
	// END OF MODELMATRIX CREATION

	// GEOMETRY COLLECTION CREATION
	glCreateBuffers(1, &mRaytracingGeometryCollection);
	glNamedBufferStorage(mRaytracingGeometryCollection, sizeof(Core::Triangle) * (static_cast<uint32_t>(1) << expOfTwo_maxModels)* (static_cast<uint32_t>(1) << expOfTwo_maxCollectionsForModel)* (static_cast<uint32_t>(1) << expOfTwo_maxGeometryOnCollection), NULL, 0);
	// END OF GEOMETRY COLLECTION CREATION

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
}

OpenGLPipeline::~OpenGLPipeline() {
	// Delete textures used to store the scene
	glDeleteBuffers(1, &mRaytracingTLAS);
	glDeleteBuffers(1, &mRaytracingBLASCollection);
	glDeleteBuffers(1, &mRaytracingGeometryCollection);
	glDeleteBuffers(1, &mRaytracingModelMatrix);

	// Avoid removing a VAO while it is currently bound
	glBindVertexArray(0);

	// Remove the HDR buffer
	glDeleteBuffers(1, &mHDRUniformBuffer);

	// Remove the camera buffer
	glDeleteBuffers(1, &mCameraUniformBuffer);

	// Remove main VAO
	glDeleteVertexArrays(1, &mQuadVAO);

	// Remove VBO
	glDeleteBuffers(1, &mQuadVBO);
}

void OpenGLPipeline::onReset() noexcept {
	Program::use(*mRaytracerFlush);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TLAS_BINDING, mRaytracingTLAS);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_BINDING, mRaytracingBLASCollection);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GEOMETRY_BINDING, mRaytracingGeometryCollection);

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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TLAS_BINDING, mRaytracingTLAS);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_BINDING, mRaytracingBLASCollection);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GEOMETRY_BINDING, mRaytracingGeometryCollection);

	// update and bind HDR parameters uniform buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, HDR_BINDING, mHDRUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Core::HDR), reinterpret_cast<const void*>(&hdr));

	// update and bind the camera uniform buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, mCameraUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Core::Camera), reinterpret_cast<const void*>(&camera));
	
	// Bind the texture to be written by the raytracer
	glBindImageTexture(OUTPUT_BINDING, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// make sure writing to render target has finished before using it
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
	glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);
	
	// The VAO with the screen quad needs to be binded only once as the raytracing never uses any other VAOs
	glBindVertexArray(mQuadVAO);

	// Bind the texture generated by raytracing
	glBindTextureUnit(OUTPUT_BINDING, mRaytracerOutputTexture);

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

void OpenGLPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitivesCollection, const GeometryInsertAttributes& insertData) noexcept {
	static_assert( (sizeof(GeometryPrimitive) == sizeof(glm::vec4) ), "Geometry type not matching input GLSL");
	
	// When creating the SSBO used to write geometry on the GPU the primitivesCollection vector will be read sequentially for the entire SSBO length, so make sure that won't generate a SEGFAULT or incorrect AABBs...
	while (primitivesCollection.size() < ((size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS))) {
		// .... by postponing empty geometry
		primitivesCollection.emplace_back(GeometryPrimitive());
	}

	Program::use(*mRaytracerInsert);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TLAS_BINDING, mRaytracingTLAS);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_BINDING, mRaytracingBLASCollection);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GEOMETRY_BINDING, mRaytracingGeometryCollection);

	// Prepare the temporary input geometry SSBO
	GLuint temporaryInputGeometry;
	glCreateBuffers(1, &temporaryInputGeometry);
	glNamedBufferStorage(temporaryInputGeometry, sizeof(glm::vec4) * (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS) * size_t(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection), primitivesCollection.data(), 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GEOMETRY_INSERT_BINDING, temporaryInputGeometry);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, GEOMETRY_INSERTT_ATTR_BINDING, mGeometryInsertAttributesUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GeometryInsertAttributes), reinterpret_cast<const void*>(&insertData));

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection, size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS, 1);

	// synchronize with the GPU: the insert procedure writes to texture (BLAS) and to the ModelMatrix SSBO.
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glDeleteBuffers(1, &temporaryInputGeometry);
}

void OpenGLPipeline::update() noexcept {
	Program::use(*mRaytracerUpdate);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TLAS_BINDING, mRaytracingTLAS);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_BINDING, mRaytracingBLASCollection);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BLAS_ATTRIBUTES_BINDING, mRaytracingModelMatrix);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GEOMETRY_BINDING, mRaytracingGeometryCollection);

	glDispatchCompute(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels, 1, 1);

	// synchronize with the GPU: the update procedure only write to texture (TLAS)
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
