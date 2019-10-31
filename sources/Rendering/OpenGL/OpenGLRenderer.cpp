#include "Rendering/OpenGL/OpenGLRenderer.h"

#include "Rendering/OpenGL/Pipeline/VertexShader.h"
#include "Rendering/OpenGL/Pipeline/FragmentShader.h"
#include "Rendering/OpenGL/Pipeline/ComputeShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::OpenGL;
using namespace Tachyon::Rendering::OpenGL::Pipeline;

#include "shaders/tonemapping.vert.h" // SHADER_TONEMAPPING_VERT, SHADER_TONEMAPPING_VERT_size
#include "shaders/tonemapping.frag.h" // SHADER_TONEMAPPING_FRAG, SHADER_TONEMAPPING_FRAG_size
#include "shaders/raytrace.comp.h" // SHADER_RAYTRACE_COMP, SHADER_RAYTRACE_COMP_size

OpenGLRenderer::OpenGLRenderer(const Core::RenderContext& scene, glm::uint32 width, glm::uint32 height) noexcept
    : Renderer(scene, std::move(width), std::move(height)),
	mRaytracer(new Pipeline::Program(
        std::initializer_list<std::shared_ptr<const Shader>>{
            std::static_pointer_cast<const Shader>(
                    std::make_shared<const ComputeShader>(
                            reinterpret_cast<const char *>(SHADER_RAYTRACE_COMP),
							SHADER_RAYTRACE_COMP_size))
	    })
	),
	mDisplayWriter(new Pipeline::Program(
		std::initializer_list<std::shared_ptr<const Shader>>{
			std::static_pointer_cast<const Shader>(
				std::make_shared<const VertexShader>(
					reinterpret_cast<const char*>(SHADER_TONEMAPPING_VERT),
					SHADER_TONEMAPPING_VERT_size)),
				std::static_pointer_cast<const Shader>(
					std::make_shared<const FragmentShader>(
						reinterpret_cast<const char*>(SHADER_TONEMAPPING_FRAG),
						SHADER_TONEMAPPING_FRAG_size))
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
	glGenBuffers(1, &mRaytracingSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRaytracingSSBO);
	glNamedBufferStorage(mRaytracingSSBO, sizeof(Tachyon::Rendering::TLAS), NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
}

void OpenGLRenderer::render(const Renderer::ShaderAlgorithm& shadingAlgo) noexcept {
	// Clear the previously rendered scene
	glClear(GL_COLOR_BUFFER_BIT);

	const auto& scene = getSceneToBeRendered();

	void* mappedMemory = glMapNamedBufferRange(mRaytracingSSBO, 0, sizeof(Tachyon::Rendering::TLAS), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	DBG_ASSERT((mappedMemory));
	Tachyon::Rendering::TLAS* tlas = reinterpret_cast<Tachyon::Rendering::TLAS*>(mappedMemory);
	scene.linearize(*tlas);
	glUnmapNamedBuffer(mRaytracingSSBO);

	// Set the raytracer program as the active one
	Program::use(*mRaytracer);

	// Bind the raytracer SSBO (the rendering context)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mRaytracingSSBO);

	// Bind the texture to be written by the raytracer
	glBindImageTexture(0, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Set the shading algorithm
	glm::uint algoUniform = 0;
	switch (shadingAlgo) {
	case Renderer::ShaderAlgorithm::HitOrMiss:
		algoUniform = 0;
		break;
	case Renderer::ShaderAlgorithm::DistanceShader:
		algoUniform = 1;
		break;
	default:
		algoUniform = 0;
	}

	mRaytracer->setUniform("shadingAlgorithm", algoUniform);

	// Set rendering information
	mRaytracer->setUniform("width", getWidth());
	mRaytracer->setUniform("height", getHeight());

	// Set Camera parameters
	mRaytracer->setUniform("mOrigin", scene.getCamera().getCameraPosition());
	mRaytracer->setUniform("mLowerLeftCorner", scene.getCamera().getLowerLeftCorner());
	mRaytracer->setUniform("mHorizontal", scene.getCamera().getHorizontal());
	mRaytracer->setUniform("mVertical", scene.getCamera().getVertical());

	// Dispatch the compute work!
	glDispatchCompute((GLuint)(getWidth()), (GLuint)(getHeight()), 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);

	// Set parameters to obtain hdr
	mDisplayWriter->setUniform("gamma", scene.getGammaCorrection());
	mDisplayWriter->setUniform("exposure", scene.getExposure());

	// Bind the texture generated by raytracing
	glBindTextureUnit(0, mRaytracerOutputTexture);

	// Bind the final VAO
	glBindVertexArray(mQuadVAO);

	// Draw the generated image while gamma-correcting it
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

OpenGLRenderer::~OpenGLRenderer() {
	// Avoid removing a VAO while it is currently bound
	glBindVertexArray(0);

	// Remove main VAO
	glDeleteVertexArrays(1, &mQuadVAO);

	// Remove VBO
	glDeleteBuffers(1, &mQuadVBO);
}

void OpenGLRenderer::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {
	glViewport(0, 0, getWidth(), getHeight());

	// Remove the previous texture to avoid GPU memory leak(s)
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