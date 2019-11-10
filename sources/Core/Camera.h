#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class Camera {
		public:
			static const glm::vec3 UPVector;
			
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
			 * Field of View in degrees.
			 */
			glm::float32 mFoV;
			glm::float32 mAspectRatio;
			glm::vec3 mUPVector;

			/**
			 * Camera position in space
			 */
			glm::vec3 mOrigin;

			/**
			 * Camera looking direction
			 */
			glm::vec3 mLookingDir;
			
			glm::vec3 mLowerLeftCorner;
			glm::vec3 mHorizontal;
			glm::vec3 mVertical;
		};
	}
}