#pragma once

#include "Ray.h"

namespace Tachyon {
	namespace Core {
		class Camera {
		public:
			Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, glm::float32 vfov, glm::float32 aspect) noexcept;

			Ray generate(glm::float32 s, glm::float32 t) const noexcept;

			glm::vec3 getCameraPosition() const noexcept;

		private:
			glm::vec3 mOrigin;
			glm::vec3 mLowerLeftCorner;
			glm::vec3 mHorizontal;
			glm::vec3 mVertical;
		};
	}
}