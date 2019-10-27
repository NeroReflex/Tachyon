#include "Renderer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

Renderer::Renderer(const Core::RenderContext& scene, glm::uint32 width, glm::uint32 height) noexcept
	: mScene(scene), mWindowWidth(std::move(width)), mWindowHeight(std::move(height)) {}

void Renderer::resize(glm::uint32 width, glm::uint32 height) noexcept {
	// Execute callback before doing anything
	onResize(mWindowWidth, mWindowHeight, width, height);

	// Change width and height values
	mWindowWidth = width;
	mWindowHeight = height;
}

glm::uint32 Renderer::getWidth() const noexcept {
	return mWindowWidth;
}

glm::uint32 Renderer::getHeight() const noexcept {
	return mWindowHeight;
}

const Core::RenderContext& Renderer::getSceneToBeRendered() const noexcept {
	return mScene;
}

void Renderer::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {}