#pragma once

#include "Core/GeometryCollection.h"
#include "PPMImage.h"

namespace Tachyon {
	namespace Rendering {
		class Renderer {
		public:
			Renderer(PPMImage& renderingSurface) noexcept;

			void render(const Core::GeometryCollection& scene) noexcept;

		private:
			PPMImage& mRenderingSurface;
		};
	}
}