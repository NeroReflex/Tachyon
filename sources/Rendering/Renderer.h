#pragma once

#include "Core/GeometryCollection.h"
#include "Rendering/RenderSurface.h"

namespace Tachyon {
	namespace Rendering {
		class Renderer {
		public:
			Renderer(RenderSurface& renderingSurface) noexcept;

			void render(const Core::GeometryCollection& scene) noexcept;

		private:
			RenderSurface& mRenderingSurface;
		};
	}
}