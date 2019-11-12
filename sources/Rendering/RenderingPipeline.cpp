#include "Rendering/RenderingPipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

// This is included to query the memory size of objects
#include "shaders/config.glsl"

RenderingPipeline::RenderingPipeline() noexcept
	: mWindowWidth(0), mWindowHeight(0),
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
	) {}

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

void RenderingPipeline::render(const Core::Camera& camera, glm::uint32 width, glm::uint32 height) noexcept {
	if ((width != getWidth()) || (height != getHeight())) resize(width, height);

	onRender(camera);
}

void RenderingPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {}
