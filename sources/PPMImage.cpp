#include "PPMImage.h"

using namespace Tachyon;

PPMImage::PPMImage(glm::uint32 width, glm::uint32 height) noexcept
	: mWidth(width),
	mHeight(height),
	mColorBuffer(static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight), glm::vec3(0, 0, 0)) {}

PPMImage::PPMImage(const PPMImage& src) noexcept
	: mWidth(src.mWidth),
	mHeight(src.mHeight),
	mColorBuffer(src.mColorBuffer) {}

PPMImage& PPMImage::operator=(const PPMImage& src) noexcept {
	if (&src != this) {
		mWidth = src.mWidth;
		mHeight = src.mHeight;
		mColorBuffer = src.mColorBuffer;
	}

	return *this;
}

glm::uint32 PPMImage::getWidth() const noexcept {
	return mWidth;
}

glm::uint32 PPMImage::getHeight() const noexcept {
	return mHeight;
}

void PPMImage::setPixel(const glm::uint32& width, const glm::uint32& height, const glm::vec3& color) noexcept {
	mColorBuffer[(static_cast<size_t>(height) * static_cast<size_t>(mWidth)) + static_cast<size_t>(width)] = glm::clamp(color, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

glm::vec3 PPMImage::getPixel(const glm::uint32& width, const glm::uint32& height) const noexcept {
	return mColorBuffer[(static_cast<size_t>(height) * static_cast<size_t>(mWidth)) + static_cast<size_t>(width)];
}

void PPMImage::resize(const glm::uint32& width, const glm::uint32& height) noexcept {
	// Do not change anything if it is not necessary
	if ((mWidth == width) && (mHeight == height)) return;

	size_t newDimension = static_cast<size_t>(width)* static_cast<size_t>(height);

	mWidth = width;
	mHeight = height;

	mColorBuffer.clear();
	mColorBuffer.reserve(newDimension);

	for (size_t i = 0; i < newDimension; ++i)
		mColorBuffer.push_back(glm::vec3(0, 0, 0));
	
	mColorBuffer.shrink_to_fit();
}

void PPMImage::write(const std::string& fileName) const noexcept {
	// Open the stream
	std::ofstream outFile(fileName);

	// Write the header
	outFile << "P3 " << mWidth << " " << mHeight << " " << std::endl << "255" << std::endl;

	for (int j = mHeight; j--;)
	{
		for (int i = 0; i < mWidth; ++i)
		{
			const auto& color = mColorBuffer[(static_cast<size_t>(j) * static_cast<size_t>(mWidth)) + static_cast<size_t>(i)];
			
			outFile
				<< static_cast<unsigned>(color.r * 255) << " "
				<< static_cast<unsigned>(color.g * 255) << " "
				<< static_cast<unsigned>(color.b * 255) << " "
				<< std::endl;
		}
	}

	// Close the stream
	outFile.close();
}