#include "Rendering/RenderingPipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

// This is included to query the memory size of objects
#include "shaders/config.glsl"

RenderingPipeline::RenderingPipeline(GLFWwindow* window) noexcept
	: mWindow(window), mWindowWidth(0), mWindowHeight(0),
	mRaytracerInfo(expOfTwo_maxModels, expOfTwo_maxCollectionsForModel, expOfTwo_maxGeometryOnCollection, expOfTwo_numOfVec4OnGeometrySerialization),
	mRaytracerRequirements(
		(size_t(1) << (mRaytracerInfo.expOfTwo_numberOfModels + 1)) * 2,
		(size_t(1) << (mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS + 1)) * 2,
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels,
		4,
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels,
		size_t(1) << (mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection + mRaytracerInfo.expOfTwo_numberOfTesselsForGeometryTexturazation),
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS,
		size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels
	) {

	if (mWindow) {
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		mWindowWidth = static_cast<glm::uint32>(windowWidth);
		mWindowHeight = static_cast<glm::uint32>(windowHeight);
	}
}

void RenderingPipeline::resize(glm::uint32 width, glm::uint32 height) noexcept {
	// Execute callback before doing anything
	onResize(mWindowWidth, mWindowHeight, width, height);

	// Change width and height values
	mWindowWidth = width;
	mWindowHeight = height;
}

glm::uint32 RenderingPipeline::getWidth() const noexcept {
	return mWindowWidth;
}

glm::uint32 RenderingPipeline::getHeight() const noexcept {
	return mWindowHeight;
}

void RenderingPipeline::reset() noexcept {
	onReset();
}

void RenderingPipeline::render(const Core::HDR& hdr, const Core::Camera& camera) noexcept {
	int windowWidth, windowHeight;
	glfwGetWindowSize(mWindow, &windowWidth, &windowHeight);

	if ((static_cast<glm::uint32>(windowWidth) != getWidth()) || (static_cast<glm::uint32>(windowHeight) != getHeight())) resize(static_cast<glm::uint32>(windowWidth), static_cast<glm::uint32>(windowHeight));

	onRender(hdr, camera);
}

void RenderingPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {}
