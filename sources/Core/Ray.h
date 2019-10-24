#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class Ray {
		public:
			Ray() = delete;

			Ray(const Ray& src) noexcept;

			Ray(Ray&&) noexcept;
			
			Ray(glm::vec3 origin, glm::vec3 direction) noexcept;

			Ray& operator=(const Ray& src) noexcept;

			glm::vec3 getOrigin() const noexcept;

			glm::vec3 getDirection() const noexcept;

			glm::vec3 pointAt(const glm::float32& coeff) const noexcept;

			const glm::vec4& getInvDirection() const noexcept;

			const std::array<glm::int16, 3>& getSigns() const noexcept;

		private:
			/**
			 * This is the ray origin.
			 * note: the W component is always 1.
			 */
			glm::vec4 mOrigin;

			/**
			 * This is the ray direction.
			 * note: the W component is always 0.
			 */
			glm::vec4 mDirection;

			glm::vec4 mInvDirection;

			std::array<glm::int16, 3> mSign;
		};
	}
}