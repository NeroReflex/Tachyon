#pragma once

#include "Tachyon.h"
#include "ToneMapping/ToneMapper.h"
#include "PPMImage.h"

namespace Tachyon {
	namespace Rendering {
		class RenderSurface {
		public:
			RenderSurface(glm::uint32 width, glm::uint32 height) noexcept;

			RenderSurface& operator=(const RenderSurface&) noexcept;

			RenderSurface(const RenderSurface&) noexcept;

			void transferTo(PPMImage& destination, ToneMapping::ToneMapper& tm) const noexcept;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

			glm::uint32 getArea() const noexcept;

			void reset() noexcept;

			void store(const glm::uint32& width, const glm::uint32& height, const glm::vec3& color, const glm::float32& depth) noexcept;

		protected:
			glm::uint32 mWidth, mHeight;

			/**
			 * This is the buffer containing the color data.
			 */
			std::vector<glm::vec3> mColorBuffer;

			/**
			 * This is the buffer containing the depth data.
			 */
			std::vector<glm::float32> mDepthBuffer;
		};
	}
}