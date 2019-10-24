#pragma once

#include "Core/TLAS.h"
#include "Core/Camera.h"
#include "Rendering/RenderSurface.h"
#include "Rendering/Shading/Shader.h"

namespace Tachyon {
	namespace Rendering {
		class Renderer {
		public:
			enum class ShaderAlgorithm {
				DistanceShader = 0,

			};

			Renderer(RenderSurface& renderingSurface) noexcept;

			void render(const Core::Camera& camera, const ShaderAlgorithm& shadingAlgo, const Core::TLAS& scene) noexcept;

		private:
			RenderSurface& mRenderingSurface;
		};
	}
}