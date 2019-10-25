#pragma once

#include "Core/TLAS.h"
#include "Core/Camera.h"
#include "Rendering/RenderSurface.h"
#include "Rendering/Shading/Shader.h"

namespace Tachyon {
	namespace Rendering {
		class ComputeRenderer {
		public:
			enum class ShaderAlgorithm {
				DistanceShader = 0,

			};

			ComputeRenderer() noexcept;

			void render(const Core::Camera& camera, const ShaderAlgorithm& shadingAlgo, const Core::TLAS& scene) noexcept;

		private:
		};
	}
}