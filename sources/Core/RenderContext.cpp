#include "Core/RenderContext.h"

using namespace Tachyon;
using namespace Tachyon::Core;

RenderContext::RenderContext(Core::Camera camera, glm::float32 gammaCorrection, glm::float32 exposure) noexcept
	: mTLAS(new TLAS()),
	mCamera(new Camera(std::move(camera))),
	mGammaCorrection(std::move(gammaCorrection)),
	mExposure(std::move(exposure)) {}

TLAS& RenderContext::getRaytracingAS() noexcept {
	return *mTLAS;
}

Camera& RenderContext::getCamera() noexcept {
	return *mCamera;
}

const TLAS& RenderContext::getRaytracingAS() const noexcept {
	return *mTLAS;
}

const Camera& RenderContext::getCamera() const noexcept {
	return *mCamera;
}

void RenderContext::setGammaCorrection(const glm::float32& gamma) noexcept {
	mGammaCorrection = gamma;
}

glm::float32 RenderContext::getGammaCorrection() const noexcept {
	return mGammaCorrection;
}

void RenderContext::setExposure(const glm::float32& exposure) noexcept {
	mExposure = exposure;
}

glm::float32 RenderContext::getExposure() const noexcept {
	return mExposure;
}