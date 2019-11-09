#include "Rendering/RenderingPipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

RenderingPipeline::RenderingPipeline() noexcept
	: mWindowWidth(0), mWindowHeight(0) {}

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

void RenderingPipeline::render(const Core::Camera& camera, glm::uint32 width, glm::uint32 height) noexcept {
	if ((width != getWidth()) || (height != getHeight())) resize(width, height);

	onRender(camera);
}

void RenderingPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {}
