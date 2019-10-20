#include "Renderer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

Renderer::Renderer(PPMImage& renderingSurface) noexcept
	: mRenderingSurface(renderingSurface) {}

void Renderer::render(const Core::GeometryCollection& scene) noexcept {
	const auto lower_left_corner = glm::vec3(-2.0, -1.0, -1.0);
	const auto horizontal = glm::vec3(4.0, 0, 0);
	const auto vertical = glm::vec3(0, 2, 0);
	const auto origin = glm::vec3(0, 0, 0);

	for (int w = 0; w < mRenderingSurface.getWidth(); ++w) {
		for (int h = 0; h < mRenderingSurface.getHeight(); ++h) {
			const auto u = static_cast<glm::float32>(w) / mRenderingSurface.getWidth();
			const auto v = static_cast<glm::float32>(h) / mRenderingSurface.getHeight();

			const Core::Ray currentCameraRay(origin, std::move(lower_left_corner + u*horizontal + v*vertical));

			const glm::vec3 currentColor = (scene.hit(currentCameraRay)) ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 0);

			mRenderingSurface.setPixel(w, h, currentColor);
		}
	}
}