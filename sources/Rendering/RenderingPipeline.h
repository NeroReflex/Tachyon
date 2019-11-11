#pragma once

#include "Core/Camera.h"
#include "GeometryPrimitive.h"

namespace Tachyon {
	namespace Rendering {

		class RenderingPipeline {
		public:
			RenderingPipeline() noexcept;

			RenderingPipeline(const RenderingPipeline&) = delete;

			RenderingPipeline(RenderingPipeline&&) = delete;

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;

			virtual ~RenderingPipeline() = default;

			virtual void enqueueModel(std::vector<GeometryPrimitive>&& primitive, GLuint location) noexcept = 0;

			void reset() noexcept;

			void render(const Core::Camera& camera, glm::uint32 width, glm::uint32 height) noexcept;

		protected:
			virtual void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

			virtual void onRender(const Core::Camera& camera) noexcept = 0;

			virtual void onReset() noexcept = 0;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

		private:
			void resize(glm::uint32 width, glm::uint32 height) noexcept;

			glm::uint32 mWindowWidth, mWindowHeight;
		};
		
	}
}