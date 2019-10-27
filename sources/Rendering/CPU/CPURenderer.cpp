#include "Rendering/CPU/CPURenderer.h"

#include "Rendering/CPU/Shading/DistanceShader.h"

#include "Rendering/CPU/ExposureToneMapper.h"

// Removed macros that generate compile errors
#undef max
#undef min

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::CPU;

CPURenderer::CPURenderer(glm::uint32 width, glm::uint32 height) noexcept
	: Renderer(std::move(width), std::move(height)),
	mRenderingSurface(RenderSurface(getWidth(), getHeight())) {}

void CPURenderer::render(const Core::RenderContext& scene, const ShaderAlgorithm& shadingAlgo) noexcept {
	const Core::Camera& camera = scene.getCamera();
	const Core::TLAS& as = scene.getRaytracingAS();

	// Reset the render target surface
	mRenderingSurface.clear();

	std::shared_ptr<Rendering::CPU::Shading::Shader> shader;
	if (shadingAlgo == ShaderAlgorithm::DistanceShader) {
		shader.reset(new Tachyon::Rendering::CPU::Shading::DistanceShader());
	}
	
	const ExposureToneMapper gammaCorrector(scene.getGammaCorrection(), scene.getExposure());

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

			mRenderingSurface.store(i, j, glm::vec4(gammaCorrector(currentColor), 1.0));
		}
	}
}

void CPURenderer::transfertResult(PPMImage& image) const noexcept {
	mRenderingSurface.transferTo(image);
}

void CPURenderer::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {
	mRenderingSurface.resize(newWidth, newHeight);
}
