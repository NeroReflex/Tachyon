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
#include "shaders/raytrace.glsl.h" // SHADER_RAYTRACE_GLSL, SHADER_RAYTRACE_GLSL_size

OpenGLRenderer::OpenGLRenderer() noexcept
    : mRaytracer(new Pipeline::Program(
        std::initializer_list<std::shared_ptr<const Shader>>{
            std::static_pointer_cast<const Shader>(
                    std::make_shared<const ComputeShader>(
                            reinterpret_cast<const char *>(SHADER_RAYTRACE_GLSL),
                            SHADER_RAYTRACE_GLSL_size))
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
	mOutputWidth(0),
	mOutputHeight(0),
	mRaytracerOutputTexture(0) {

	std::array<glm::vec2, 3 * 2> screenTrianglesPosition = {
		// Lower triangle
		glm::vec2(-1, -1),
		glm::vec2(1, -1),
		glm::vec2(1, 1),

		// Higher triangle
		glm::vec2(-1, -1),
		glm::vec2(-1, 1),
		glm::vec2(1, 1)
	};

	std::array<glm::vec2, 3 * 2> screenTrianglesUV = {
		// Lower triangle
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		// Higher triangle
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1)
	};

	// Create the final VAO
	glCreateVertexArrays(1, &mFinalVAO);
	glBindVertexArray(mFinalVAO);

	// Create final VBOs
	glCreateBuffers(mVerticesVBO.size(), mVerticesVBO.data());

	// Finalize VBOs
	glNamedBufferStorage(mVerticesVBO[0], sizeof(glm::vec2) * screenTrianglesPosition.size(), screenTrianglesPosition.data(), 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBO[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexArrayAttrib(mFinalVAO, 0);

	glNamedBufferStorage(mVerticesVBO[1], sizeof(glm::vec2) * screenTrianglesPosition.size(), screenTrianglesPosition.data(), 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBO[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexArrayAttrib(mFinalVAO, 1);

	// Create the VBO used to store the raytrace structure
	glCreateBuffers(1, &mTLAS);
	glNamedBufferStorage(mTLAS, Core::TLAS::getBufferSize(), NULL, GL_MAP_WRITE_BIT);

	// Print info about OpenGL Compute
	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);

}

void OpenGLRenderer::render(const Core::RenderContext& scene, const Renderer::ShaderAlgorithm& shadingAlgo) noexcept {
	// Set the raytracer program as the active one
	Program::use(*mRaytracer);

	// dimensions of the render target
	const int tex_w = 480, tex_h = 480;

	if ((tex_w != mOutputWidth) || (tex_h != mOutputHeight)) {
		// Update width and height values
		mOutputWidth = tex_w;
		mOutputHeight = tex_h;

		// Remove the previous texture to avoid GPU memory leak(s)
		if ((mOutputWidth > 0) || (mOutputHeight > 0))
			glDeleteTextures(1, &mRaytracerOutputTexture);

		// Create a new texture 
		glGenTextures(1, &mRaytracerOutputTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mRaytracerOutputTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mOutputWidth, mOutputHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}
	/*
	// Fill the AS structure on the GPU
	auto mappedMemory = glMapNamedBufferRange(mTLAS, 0, Core::TLAS::getBufferSize(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	DBG_ASSERT( (mappedMemory != nullptr) );
	Core::TLAS::linearizeToBuffer(scene.getRaytracingAS(), mappedMemory);
	glUnmapNamedBuffer(mTLAS);

	// Bind the AS to the current compute shader
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, mTLAS, 0, Core::TLAS::getBufferSize());
	*/

	Program::use(*mRaytracer);
	mRaytracer->setUniform("width", glm::uint32(mOutputWidth));
	mRaytracer->setUniform("height", glm::uint32(mOutputHeight));

	// Dispatch the compute work!
	glDispatchCompute((GLuint)mOutputWidth, (GLuint)mOutputHeight, 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Switch to the tone mapper program
	Program::use(*mDisplayWriter);

	mDisplayWriter->setUniform("gamma", glm::float32(2.2));
	mDisplayWriter->setUniform("exposure", glm::float32(0.2));

	// Bind the texture generated by raytracing
	glBindImageTexture(0, mRaytracerOutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Bind the final VAO
	glBindVertexArray(mFinalVAO);

	// Draw the generated image (gamma corrected, hopefully...)
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
}

OpenGLRenderer::~OpenGLRenderer() {
	// Avoid removing a VAO while it is currently bound
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &mFinalVAO);

	// Remove VBO
	glDeleteBuffers(mVerticesVBO.size(), mVerticesVBO.data());
}