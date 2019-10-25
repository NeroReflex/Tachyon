#pragma once

#include "Core/RenderContext.h"

namespace Tachyon {
	namespace Rendering {
		class Renderer {
		public:
			enum class ShaderAlgorithm {
				DistanceShader = 0,

			};

			virtual void render(const Core::RenderContext& scene, const ShaderAlgorithm& shadingAlgo) noexcept = 0;
		};
	}
}