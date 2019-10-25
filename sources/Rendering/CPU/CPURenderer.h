#pragma once

#include "Rendering/Renderer.h"
#include "Rendering/CPU/RenderSurface.h"
#include "Rendering/CPU/Shading/Shader.h"

namespace Tachyon {
	namespace Rendering {
		namespace CPU {

			class CPURenderer :
				virtual public Rendering::Renderer {

			public:
				CPURenderer() noexcept;

				void render(const Core::RenderContext& scene, const ShaderAlgorithm& shadingAlgo) noexcept final;

			private:
				RenderSurface mRenderingSurface;
			};
		}
	}
}