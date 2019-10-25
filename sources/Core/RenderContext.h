#pragma once

#include "Core/TLAS.h"
#include "Core/Camera.h"

namespace Tachyon {
	namespace Core {

		class RenderContext {

		public:
			RenderContext(Core::Camera camera) noexcept;

			const TLAS& getRaytracingAS() const noexcept;

			const Camera& getCamera() const noexcept;

		private:
			std::unique_ptr<TLAS> mTLAS;

			std::unique_ptr<Core::Camera> mCamera;
		};
	}
}