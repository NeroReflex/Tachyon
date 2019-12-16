#pragma once

#include "Tachyon.h"

namespace Tachyon {
    namespace Core {

        /**
         * Represents a triangle.
         */
        class Triangle {
            
            alignas(sizeof(glm::vec4)) glm::vec4 vertices[3];

        public:
			Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

			Triangle(const Triangle& src) noexcept;

			Triangle& operator=(const Triangle& src) noexcept;

            ~Triangle() = default;
        };

    }
}
