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

OpenGLPipeline::OpenGLPipeline() noexcept
    : RenderingPipeline(),
	mRaytracerFlush(
		new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_flush_compOGL), raytrace_flush_compOGL_size)
		})
	),
	mRaytracerInsert(new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(raytrace_insert_compOGL), raytrace_insert_compOGL_size)
		})
	),
	/*mRaytracerRender(new Pipeline::Program(
        std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const ComputeShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char *>(raytrace_compOGL), raytrace_compOGL_size, shadingAlgoEntry[static_cast<int>(shadingAlgo)])
		})
	),*/
	mDisplayWriter(new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::make_shared<const VertexShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(tonemapping_vertOGL), tonemapping_vertOGL_size),
			std::make_shared<const FragmentShader>(Shader::SourceType::SPIRV, reinterpret_cast<const char*>(tonemapping_fragOGL), tonemapping_fragOGL_size)
		})
    ),
	mRaytracerOutputTexture(0) {

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

	// Active the texture unit
	glActiveTexture(GL_TEXTURE0);

	// Set the initial viewport
	glViewport(0, 0, getWidth(), getHeight());

	// Create a new 2D texture used to store the raw raytrace result (without gamma correction)
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerOutputTexture);
	glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
	glTextureStorage2D(mRaytracerOutputTexture, 1, GL_RGBA32F, getWidth(), getHeight());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

	//Create the raytracer SSBO, with corret dimensions
	glGenBuffers(mRaytracingSSBO.size(), mRaytracingSSBO.data());
	std::for_each(mRaytracingSSBO.cbegin(), mRaytracingSSBO.cend(), [](const GLuint& ssbo) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	});
	
	// This is the collection of geometry
	glNamedBufferStorage(mRaytracingSSBO[0], sizeof(BLASGeometryCollection) * (1 << expOfTwo_maxModels), NULL, 0);

	// This is the BLAS
	glNamedBufferStorage(mRaytracingSSBO[1], sizeof(BLAS) * (1 << expOfTwo_maxModels), NULL, 0);
	
	// This is the TLAS
	glNamedBufferStorage(mRaytracingSSBO[2], sizeof(TLAS), NULL, 0);
	
	// This is the global collection of geometry
	//glNamedBufferStorage(mRaytracingSSBO[3], sizeof(glm::mat4) * maxNumberOfBLASInTLAS, NULL, 0);

	// The VAO with the screen quad needs to be binded only once as the raytracing never uses any other VAOs
	glBindVertexArray(mQuadVAO);

	// Prepare the scene
	flush();
}

void OpenGLPipeline::prapareDispatch() noexcept {
	// Bind the raytracer SSBO (the rendering context)
	for (GLuint k = 0; k < mRaytracingSSBO.size(); ++k)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, k, mRaytracingSSBO[k]);
}

void OpenGLPipeline::onRender() noexcept {
	// Clear the previously rendered scene
	glClear(GL_COLOR_BUFFER_BIT);
	/*
	// frame-by-frame reserialization
	void* blasMappedMemory = glMapNamedBufferRange(mRaytracingSSBO[0], 0, sizeof(Tachyon::Rendering::BLAS) * maxNumberOfBLASInTLAS, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	DBG_ASSERT((blasMappedMemory));
	Tachyon::Rendering::BLAS* blasMapped = reinterpret_cast<Tachyon::Rendering::BLAS*>(blasMappedMemory);

	void* treeMappedMemory = glMapNamedBufferRange(mRaytracingSSBO[1], 0, sizeof(Tachyon::Rendering::NodeData) * maxNumberOfTreeElementsInTLAS, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	DBG_ASSERT((treeMappedMemory));
	Tachyon::Rendering::NodeData* treeMapped = reinterpret_cast<Tachyon::Rendering::NodeData*>(treeMappedMemory);

	void* modelMappedMemory = glMapNamedBufferRange(mRaytracingSSBO[2], 0, sizeof(glm::mat4) * maxNumberOfBLASInTLAS, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	DBG_ASSERT((modelMappedMemory));
	glm::mat4* modelMapped = reinterpret_cast<glm::mat4*>(modelMappedMemory);

	// Linearize data
	scene.getRaytracingAS().linearize(blasMapped, modelMapped, treeMapped);
	*/
	// Unmap memory
	glUnmapNamedBuffer(mRaytracingSSBO[0]);
	glUnmapNamedBuffer(mRaytracingSSBO[1]);
	glUnmapNamedBuffer(mRaytracingSSBO[2]);
	
	// Set the raytracer program as the active one
	Program::use(*mRaytracerRender);

	// Bind the raytracer SSBO (the rendering context)
	prapareDispatch();

	// Bind the texture to be written by the raytracer
	glBindImageTexture(0, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Set rendering information
	mRaytracerRender->setUniform("width", getWidth());
	mRaytracerRender->setUniform("height", getHeight());

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);

	// Set parameters to obtain hdr
	//mDisplayWriter->setUniform("gamma", scene.getGammaCorrection());
	//mDisplayWriter->setUniform("exposure", scene.getExposure());

	// Bind the texture generated by raytracing
	glBindTextureUnit(0, mRaytracerOutputTexture);

	// Draw the generated image while gamma-correcting it
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

OpenGLPipeline::~OpenGLPipeline() {
	// Avoid removing a VAO while it is currently bound
	glBindVertexArray(0);

	// Remove main VAO
	glDeleteVertexArrays(1, &mQuadVAO);

	// Remove VBO
	glDeleteBuffers(1, &mQuadVBO);
}

void OpenGLPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {
	glViewport(0, 0, getWidth(), getHeight());

	// Remove the previous texture to avoid GPU memory leak(s)
	if (mRaytracerOutputTexture)
		glDeleteTextures(1, &mRaytracerOutputTexture);

	// Create a new 2D texture used to store the raw raytrace result (without gamma correction)
	glCreateTextures(GL_TEXTURE_2D, 1, &mRaytracerOutputTexture);
	glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
	glTextureStorage2D(mRaytracerOutputTexture, 1, GL_RGBA32F, getWidth(), getHeight());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
}

void OpenGLPipeline::flush() noexcept {
	prapareDispatch();

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// synchronize with the GPU
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}