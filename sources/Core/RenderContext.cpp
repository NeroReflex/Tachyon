#include "Core/RenderContext.h"

using namespace Tachyon;
using namespace Tachyon::Core;

RenderContext::RenderContext(Core::Camera camera) noexcept
	: mTLAS(new TLAS()), mCamera(new Camera(std::move(camera))) {}

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