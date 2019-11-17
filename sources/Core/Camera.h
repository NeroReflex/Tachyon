#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class Camera {
		public:
			constexpr static glm::vec3 UPVector = glm::vec3(0, 1, 0);
			constexpr static glm::vec3 UPReversedVector = glm::vec3(0, -1, 0);
			
			Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::float32 vfov, glm::float32 aspect, glm::vec3 vup = Camera::UPVector) noexcept;

			glm::vec3 getCameraPosition() const noexcept;

			glm::vec3 getViewDirection() const noexcept;

			glm::vec3 getUp() const noexcept;

			glm::float32 getFieldOfView() const noexcept;

			glm::float32 getAspectRatio() const noexcept;

			glm::vec3 getRaytracingLowerLeftCorner() const noexcept;

			glm::vec3 getRaytracingHorizontal() const noexcept;

			glm::vec3 getRaytracingVertical() const noexcept;

			void setAspect(glm::float32 aspect) noexcept;

			void setAspect(glm::uint32 width, glm::uint32 height) noexcept;

		private:
			void update() noexcept;

			/**
			 * Camera position in space
			 */
			alignas(sizeof(glm::vec4)) glm::vec3 mOrigin;

			/**
			 * Camera looking direction
			 */
			alignas(sizeof(glm::vec4)) glm::vec3 mLookingDir;

			/**
			 * Camera UP vector use (0, -1, 0) to reverse
			 */
			alignas(sizeof(glm::vec4)) glm::vec3 mUPVector;
			
			/**
			 * Field of View in degrees.
			 */
			alignas(sizeof(glm::float32)) glm::float32 mFoV;

			/**
			 * Aspect ratio (16:9 and so on...)
			 */
			alignas(sizeof(glm::float32)) glm::float32 mAspectRatio;


			glm::vec3 mLowerLeftCorner;
			glm::vec3 mHorizontal;
			glm::vec3 mVertical;
		};
	}
}