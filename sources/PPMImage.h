#pragma once

#include "Tachyon.h"

namespace Tachyon {
	class PPMImage {
	public:
		PPMImage() = delete;

		PPMImage(PPMImage&&) = delete;

		PPMImage(const PPMImage&) noexcept;

		PPMImage& operator=(const PPMImage&) noexcept;

		PPMImage(glm::uint32 width, glm::uint32 height) noexcept;

		void write(const std::string& fileName) const noexcept;

		glm::uint32 getWidth() const noexcept;

		glm::uint32 getHeight() const noexcept;

		void setPixel(const glm::uint32& width, const glm::uint32& height, const glm::vec3& color) noexcept;

		glm::vec3 getPixel(const glm::uint32& width, const glm::uint32& height) const noexcept;

	private:
		glm::uint32 mWidth, mHeight;

		/**
		 * This is the buffer containing the color data.
		 */
		std::vector<glm::vec3> mColorBuffer;
	};
}