#pragma once

#include "Core/RenderContext.h"

namespace Tachyon {
	namespace Rendering {

		class Renderer {
		public:
			enum class ShaderAlgorithm {
				DistanceShader = 0,

			};

			Renderer() = delete;

			Renderer(const Renderer&) = delete;

			Renderer(Renderer&&) = delete;

			Renderer& operator=(const Renderer&) = delete;

			virtual ~Renderer() = default;

			Renderer(glm::uint32 width, glm::uint32 height) noexcept;

			void resize(glm::uint32 width, glm::uint32 height) noexcept;

			virtual void render(const Core::RenderContext& scene, const Renderer::ShaderAlgorithm& shadingAlgo) noexcept = 0;

		protected:
			virtual void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

		private:
			glm::size_t mWindowWidth, mWindowHeight;
		};
		
	}
}