#include "Renderer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;

Renderer::Renderer(RenderSurface& renderingSurface) noexcept
	: mRenderingSurface(renderingSurface) {}

void Renderer::render(const Core::Camera& camera, const Core::GeometryCollection& scene) noexcept {
	// Reset the render target surface
	mRenderingSurface.reset();

	for (size_t j = 0; j < mRenderingSurface.getHeight(); ++j) {
		for (size_t i = 0; i < mRenderingSurface.getWidth(); ++i) {
			const auto u = static_cast<glm::float32>(i) / static_cast<glm::float32>(mRenderingSurface.getWidth());
			const auto v = static_cast<glm::float32>(j) / static_cast<glm::float32>(mRenderingSurface.getHeight());

			const Core::Ray currentCameraRay = camera.generate(u, v);

			// Hit informations
			Core::RayGeometryIntersection isect;

			const auto currentColor = (scene.intersection(currentCameraRay, 0.00001, std::numeric_limits<glm::float32>::max(), isect))
				? glm::vec3(std::max<glm::float32>(0, glm::dot(isect.getNormal(), glm::normalize(camera.getCameraPosition() - isect.getPoint()))))  // HIT: 
				: glm::vec3(0, 0, 0); // MISS

			mRenderingSurface.store(i, j, currentColor, isect.getDistance());
		}
	}
}
