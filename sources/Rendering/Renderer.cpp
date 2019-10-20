#include "Renderer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

Renderer::Renderer(RenderSurface& renderingSurface) noexcept
	: mRenderingSurface(renderingSurface) {}

void Renderer::render(const Core::GeometryCollection& scene) noexcept {
	// Reset the render target surface
	mRenderingSurface.reset();

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

			// Hit informations
			Core::RayGeometryIntersection isect;

			const auto currentColor = (scene.intersection(currentCameraRay, isect)) ? isect.getNormal() : glm::vec3(0, 0, 0);

			mRenderingSurface.store(i, j, currentColor, isect.getDistance());
		}
	}
}
