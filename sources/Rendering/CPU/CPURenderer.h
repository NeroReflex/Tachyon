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
				CPURenderer(const CPURenderer&) = delete;

				CPURenderer(CPURenderer&&) = delete;

				CPURenderer& operator=(const CPURenderer&) = delete;

				~CPURenderer() override = default;

				CPURenderer(glm::uint32 width, glm::uint32 height) noexcept;

				void render(const Core::RenderContext& scene, const Renderer::ShaderAlgorithm& shadingAlgo) noexcept final;

				void transfertResult(PPMImage& image) const noexcept;

			protected:
				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept override;

			private:
				RenderSurface mRenderingSurface;
			};
		}
	}
}