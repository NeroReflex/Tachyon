#pragma once

#include "Core/RenderContext.h"

namespace Tachyon {
	namespace Rendering {

		class RenderingPipeline {
		public:
			enum ShadingAlgorithm {
				HitOrMiss = 0, // a simple cameraRay-geometry intersection algorithm
				DistanceShader = 1, // shade the geometry with the distance to the camera
			};
			
			RenderingPipeline(ShadingAlgorithm shadingAlgo = ShadingAlgorithm::DistanceShader) noexcept;

			RenderingPipeline(const RenderingPipeline&) = delete;

			RenderingPipeline(RenderingPipeline&&) = delete;

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;

			virtual ~RenderingPipeline() = default;

			void render(glm::uint32 width, glm::uint32 height) noexcept;

		protected:
			virtual void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

			virtual void onRender() noexcept = 0;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

		private:
			void resize(glm::uint32 width, glm::uint32 height) noexcept;

			glm::uint32 mWindowWidth, mWindowHeight;
		};
		
	}
}