#pragma once

#include "Core/RenderContext.h"

namespace Tachyon {
	namespace Rendering {

		class Renderer {
		public:
			enum class ShaderAlgorithm {
				HitOrMiss = 0, // a simple cameraRay-geometry intersection algorithm
				DistanceShader = 1, // shade the geometry with the distance to the camera

			};

			Renderer() = delete;

			Renderer(const Renderer&) = delete;

			Renderer(Renderer&&) = delete;

			Renderer& operator=(const Renderer&) = delete;

			virtual ~Renderer() = default;

			Renderer(const Core::RenderContext& scene, glm::uint32 width, glm::uint32 height) noexcept;

			void resize(glm::uint32 width, glm::uint32 height) noexcept;

			virtual void render(const Renderer::ShaderAlgorithm& shadingAlgo) noexcept = 0;

		protected:
			virtual void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

			const Core::RenderContext& getSceneToBeRendered() const noexcept;

		private:
			const Core::RenderContext& mScene;

			glm::size_t mWindowWidth, mWindowHeight;
		};
		
	}
}