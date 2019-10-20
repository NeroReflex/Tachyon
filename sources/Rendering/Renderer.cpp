#include "Renderer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

Renderer::Renderer(PPMImage& renderingSurface) noexcept
	: mRenderingSurface(renderingSurface) {}

void Renderer::render(const Core::GeometryCollection& scene) noexcept {
	const glm::float32 k = 2.0;

	const auto lower_left_corner = glm::vec3(-2.0, -1.0, -1.0);
	const auto horizontal = glm::vec3(k * (static_cast<glm::float32>(mRenderingSurface.getWidth()) / static_cast<glm::float32>(mRenderingSurface.getHeight())), 0, 0);
	const auto vertical = glm::vec3(0, k, 0);
	const auto origin = glm::vec3(0, 0, 0);

	for (size_t j = 0; j < mRenderingSurface.getHeight(); ++j) {
		for (size_t i = 0; i < mRenderingSurface.getWidth(); ++i) {
			const auto u = static_cast<glm::float32>(i) / static_cast<glm::float32>(mRenderingSurface.getWidth());
			const auto v = static_cast<glm::float32>(j) / static_cast<glm::float32>(mRenderingSurface.getHeight());

			const Core::Ray currentCameraRay(origin, std::move(lower_left_corner + u*horizontal + v*vertical));

			const auto currentColor = (scene.isHitBy(currentCameraRay)) ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 0);

			mRenderingSurface.setPixel(i, j, currentColor);
		}
	}
}
