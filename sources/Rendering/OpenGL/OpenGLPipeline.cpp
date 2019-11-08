#include "Rendering/OpenGL/OpenGLPipeline.h"

#include "Rendering/OpenGL/Pipeline/VertexShader.h"
#include "Rendering/OpenGL/Pipeline/FragmentShader.h"
#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

#include "Rendering/CoreTypes.h"

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


OpenGLPipeline::OpenGLPipeline() noexcept
    : RenderingPipeline(),
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
	glTextureStorage1D(mRaytracingTLAS, 1, GL_RGBA32F, (1 << (expOfTwo_maxModels + 1)) * 2);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF TLAS TEXTURE CREATION

	// BLAS COLLECTION TEXTURE CREATION
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracingBLASCollection);
	glBindTexture(GL_TEXTURE_2D, mRaytracingBLASCollection);
	glTextureStorage2D(mRaytracingBLASCollection, 1, GL_RGBA32F, (1 << (expOfTwo_maxCollectionsForModel + 1)) * 2, 1 << expOfTwo_maxModels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	// END OF BLAS COLLECTION TEXTURE CREATION
	

	//Create the raytracer SSBO, with corret dimensions
	glCreateBuffers(mRaytracingSSBO.size(), mRaytracingSSBO.data());
	std::for_each(mRaytracingSSBO.cbegin(), mRaytracingSSBO.cend(), [](const GLuint& ssbo) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	});
	
	// This is the collection of geometry
	glNamedBufferStorage(mRaytracingSSBO[0], sizeof(BLASGeometryCollection) * (1 << expOfTwo_maxModels), NULL, 0);

	// This is the BLAS
	glNamedBufferStorage(mRaytracingSSBO[1], sizeof(glm::mat4) * (1 << expOfTwo_maxModels), NULL, 0);
	
	// Prepare the tomporary input geometry SSBO
	glCreateBuffers(1, &mInputGeometryTemporary);
	glNamedBufferStorage(mInputGeometryTemporary,
		sizeof(glm::vec4) * (size_t(1) << expOfTwo_maxCollectionsForModel) * size_t(size_t(1) << expOfTwo_maxCollectionsForModel),
		NULL,
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	// The VAO with the screen quad needs to be binded only once as the raytracing never uses any other VAOs
	glBindVertexArray(mQuadVAO);
}

OpenGLPipeline::~OpenGLPipeline() {
	glDeleteTextures(1, &mRaytracingTLAS);
	glDeleteTextures(1, &mRaytracingBLASCollection);

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

void OpenGLPipeline::prapareDispatch() noexcept {
	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// Bind the raytracer SSBO (the rendering context)
	for (GLuint k = 0; k < mRaytracingSSBO.size(); ++k)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, k, mRaytracingSSBO[k]);
}

void OpenGLPipeline::onRender() noexcept {
	// Clear the previously rendered scene
	glClear(GL_COLOR_BUFFER_BIT);

	// Update the TLAS before rendering
	update();

	// Set the raytracer program as the active one
	Program::use(*mRaytracerRender);

	// Bind the raytracer SSBO (the rendering context)
	prapareDispatch();

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
	glBindTextureUnit(0, mRaytracerOutputTexture);

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

	prapareDispatch();

	// Dispatch the compute work!
	glDispatchCompute(1 << expOfTwo_maxModels, 1, 1);

	// synchronize with the GPU
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void OpenGLPipeline::enqueueModel(const std::vector<GeometryPrimitive>& primitive, GLuint location) noexcept {
	DBG_ASSERT( (location < (1 << expOfTwo_maxModels)) );

	static_assert( (sizeof(GeometryPrimitive) == sizeof(glm::vec4) ), "Geometry type not matching input GLSL");

	size_t fixedGeometryCount = (size_t(1) << expOfTwo_maxGeometryOnCollection) * (size_t(1) << expOfTwo_maxCollectionsForModel);

	GeometryPrimitive* inputGeometryMemory = reinterpret_cast<GeometryPrimitive*>(
		glMapNamedBufferRange(mInputGeometryTemporary, 0, sizeof(GeometryPrimitive) * fixedGeometryCount, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT)
	);
	DBG_ASSERT((inputGeometryMemory));
	
	for (size_t i = 0; i < fixedGeometryCount; ++i) {
		inputGeometryMemory[i] = (i < primitive.size()) ? primitive[i] : GeometryPrimitive();
	}

	glUnmapNamedBuffer(mInputGeometryTemporary);

	glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
	glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// As last step call the insertion algorithm on the GPU
	insert(location);
}

void OpenGLPipeline::insert(GLuint targetBLAS) noexcept {
	Program::use(*mRaytracerInsert);

	prapareDispatch();

	// The TLAS may get updated after an insert
	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// The BLAS gets written, and since a node is a join of left and right childs the BLAS is also read
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	mRaytracerInsert->setUniform("targetBLAS", targetBLAS);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mInputGeometryTemporary);

	// Dispatch the compute work!
	glDispatchCompute(1 << expOfTwo_maxGeometryOnCollection, 1 << expOfTwo_maxCollectionsForModel, 1);

	// synchronize with the GPU: the insert procedure writes to texture (BLAS) and to the ModelMatrix SSBO.
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void OpenGLPipeline::update() noexcept {
	Program::use(*mRaytracerUpdate);

	prapareDispatch();

	// The TLAS gets written, and since a node is a join of left and right childs the TLAS is also read
	glBindImageTexture(0, mRaytracingTLAS, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// The BLAS is only read on the update procedure
	glBindImageTexture(1, mRaytracingBLASCollection, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	// Dispatch the compute work!
	glDispatchCompute(static_cast<GLuint>(1) << expOfTwo_maxModels, 1, 1);

	// synchronize with the GPU: the update procedure only write to texture (TLAS)
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
