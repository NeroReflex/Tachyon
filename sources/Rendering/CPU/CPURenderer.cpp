#include "Rendering/CPU/CPURenderer.h"

#include "Rendering/CPU/Shading/DistanceShader.h"

#include "Rendering/CPU/ToneMapping/ExposureToneMapper.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::CPU;

CPURenderer::CPURenderer() noexcept
	: mRenderingSurface(RenderSurface(480, 360)) {}

void CPURenderer::render(const Core::RenderContext& scene, const ShaderAlgorithm& shadingAlgo) noexcept {
	const Core::Camera& camera = scene.getCamera();
	const Core::TLAS& as = scene.getRaytracingAS();

	// Reset the render target surface
	mRenderingSurface.reset();

	std::shared_ptr<Rendering::CPU::Shading::Shader> shader;
	if (shadingAlgo == ShaderAlgorithm::DistanceShader) {
		shader.reset(new Tachyon::Rendering::CPU::Shading::DistanceShader());
	}

	for (size_t j = 0; j < mRenderingSurface.getHeight(); ++j) {
		for (size_t i = 0; i < mRenderingSurface.getWidth(); ++i) {
			const auto u = static_cast<glm::float32>(i) / static_cast<glm::float32>(mRenderingSurface.getWidth());
			const auto v = static_cast<glm::float32>(j) / static_cast<glm::float32>(mRenderingSurface.getHeight());

			// Generate the current camera ray
			const Core::Ray currentCameraRay = camera.generate(u, v);

			// Hit informations
			Core::RayGeometryIntersection isect;

			const auto currentColor = (as.intersection(currentCameraRay, 0.00001, std::numeric_limits<glm::float32>::max(), isect))
				? (*shader)(isect)  // HIT: shade the point
				: glm::vec3(0, 0, 0); // MISS

			mRenderingSurface.store(i, j, currentColor, isect.getDistance());
		}
	}
}

void CPURenderer::transfertResult(PPMImage& image) const noexcept {
	Tachyon::Rendering::CPU::ToneMapping::ExposureToneMapper toneMapper(0.1);

	mRenderingSurface.transferTo(image, toneMapper);
}
