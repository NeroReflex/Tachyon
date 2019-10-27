#include "RenderSurface.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::CPU;

RenderSurface::RenderSurface(glm::uint32 width, glm::uint32 height) noexcept
	: mWidth(width),
	mHeight(height),
	mColorBuffer(static_cast<size_t>(mWidth)* static_cast<size_t>(mHeight), glm::vec4(0, 0, 0, 0)) {}

RenderSurface::RenderSurface(const RenderSurface& src) noexcept
	: mWidth(src.mWidth),
	mHeight(src.mHeight),
	mColorBuffer(src.mColorBuffer) {}

RenderSurface& RenderSurface::operator=(const RenderSurface& src) noexcept {
	if (&src != this) {
		mWidth = src.mWidth;
		mHeight = src.mHeight;
		mColorBuffer = src.mColorBuffer;
	}

	return *this;
}

void RenderSurface::resize(glm::uint32 width, glm::uint32 height) noexcept {
	mWidth = width;
	mHeight = height;

	const auto area = getArea();

	mColorBuffer.clear();
	mColorBuffer.resize(area);
	
	for (int i = 0; i < area; ++i)
		mColorBuffer.emplace_back(glm::vec4(0, 0, 0, 0));
}

glm::uint32 RenderSurface::getWidth() const noexcept {
	return mWidth;
}

glm::uint32 RenderSurface::getHeight() const noexcept {
	return mHeight;
}

glm::uint32 RenderSurface::getArea() const noexcept {
	return mWidth * mHeight;
}

void RenderSurface::clear() noexcept {
	for (size_t j = 0; j < getHeight(); ++j) {
		for (size_t i = 0; i < getWidth(); ++i) {
			auto& surfacePointColor = mColorBuffer[(static_cast<size_t>(j)* static_cast<size_t>(mWidth)) + static_cast<size_t>(i)];

			surfacePointColor = glm::vec4(0, 0, 0, 0);
		}
	}
}

void RenderSurface::transferTo(PPMImage& destination) const noexcept {
	destination.resize(mWidth, mHeight);

	for (size_t j = 0; j < getHeight(); ++j)
		for (size_t i = 0; i < getWidth(); ++i)
			destination.setPixel(i, j, mColorBuffer[(static_cast<size_t>(j)* static_cast<size_t>(mWidth)) + static_cast<size_t>(i)]);
}

void RenderSurface::store(const glm::uint32& width, const glm::uint32& height, const glm::vec4& color) noexcept {
	mColorBuffer[(static_cast<size_t>(height)* static_cast<size_t>(mWidth)) + static_cast<size_t>(width)] = color;
}