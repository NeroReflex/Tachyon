#pragma once

#include "Core/TLAS.h"
#include "Core/Camera.h"

namespace Tachyon {
	namespace Core {

		/**
		 * This class represents a scene to be rendered using a raytracing algorithm.
		 */
		class RenderContext {

		public:
			constexpr static glm::float32 defaultGammaCorrection = 2.2;
			constexpr static glm::float32 defaultExposure = 0.2;

			RenderContext(Core::Camera camera, glm::float32 gammaCorrection = defaultGammaCorrection, glm::float32 exposure = defaultExposure) noexcept;

			TLAS& getRaytracingAS() noexcept;

			const TLAS& getRaytracingAS() const noexcept;

			Camera& getCamera() noexcept;

			const Camera& getCamera() const noexcept;

			void setGammaCorrection(const glm::float32& gamma) noexcept;

			glm::float32 getGammaCorrection() const noexcept;

			void setExposure(const glm::float32& exposure) noexcept;

			glm::float32 getExposure() const noexcept;

		private:
			std::unique_ptr<TLAS> mTLAS;

			std::unique_ptr<Core::Camera> mCamera;

			glm::float32 mGammaCorrection, mExposure;
		};
	}
}