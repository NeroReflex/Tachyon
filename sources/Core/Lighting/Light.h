#pragma once

#include "Tachyon.h"

namespace Tachyon {
    namespace Core {
        namespace Lighting {
            class Light {
            
            public:
				Light(const glm::vec3& color, const glm::float32& intensity) noexcept;

                glm::vec3 getColor() const noexcept;

                glm::float32 getIntensity() const noexcept;
				

            private:
                /**
                 * mColor.xyz = RGB color
                 * mColor.w = intensity
                 */
                glm::vec4 mColor;
            };
        }
    }
}