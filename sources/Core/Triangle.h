#pragma once

#include "Tachyon.h"

namespace Tachyon {
    namespace Core {

        /**
         * Represents a triangle.
         */
        class Triangle {
            
            /**
             * xyz => sphere center
             * w => sphere radius
             */
            alignas(sizeof(glm::vec4)) glm::vec4 vertices[3];

        public:
			/*Triangle(glm::vec3 position = glm::vec3(0, 0, 0), glm::float32 radius = 0) noexcept;

			Triangle(const Triangle& src) noexcept;

			Triangle& operator=(const Triangle& src) noexcept;

            ~Triangle() = default;*/
        };

    }
}
