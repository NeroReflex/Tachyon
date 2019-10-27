#pragma once

#include "PPMImage.h"

namespace Tachyon {
	namespace Rendering {
		namespace CPU {

			class RenderSurface {
			public:
				RenderSurface(glm::uint32 width, glm::uint32 height) noexcept;

				RenderSurface& operator=(const RenderSurface&) noexcept;

				RenderSurface(const RenderSurface&) noexcept;

				void transferTo(PPMImage& destination) const noexcept;

				void resize(glm::uint32 width, glm::uint32 height) noexcept;

				glm::uint32 getWidth() const noexcept;

				glm::uint32 getHeight() const noexcept;

				glm::uint32 getArea() const noexcept;

				void clear() noexcept;

				void store(const glm::uint32& width, const glm::uint32& height, const glm::vec4& color) noexcept;

			protected:
				glm::uint32 mWidth, mHeight;

				/**
				 * This is the buffer containing the color data.
				 */
				std::vector<glm::vec4> mColorBuffer;
			};

		}
	}
}