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

			/**
			 * Returns the number of vec4 needed to linearize the render context.
			 *
			 * @return the number of vec4 occupied elements.
			 */
			constexpr static size_t linearSizeInVec4() noexcept {
				return 1;
			}

			/**
			 * Linearize the given render context to a buffer of vec4 elements.
			 *
			 * @param src the render context to be linearized
			 * @param destination a buffer of linearSizeInVec4() vec4 elements to be filled
			 */
			static void linearize(const RenderContext& src, glm::vec4* destination) noexcept {

			}

		private:
			std::unique_ptr<TLAS> mTLAS;

			std::unique_ptr<Core::Camera> mCamera;

			glm::float32 mGammaCorrection, mExposure;
		};
	}
}