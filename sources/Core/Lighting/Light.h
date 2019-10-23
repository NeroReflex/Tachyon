#pragma once

#include "Core/Linearizable.h"

namespace Tachyon {
    namespace Core {
        namespace Lighting {
            class Light :
                virtual public Linearizable {
            
            public:
                Linearizable(const glm::vec3& mColor, const glm::float32& intensity) noexcept;

                glm::vec3 getColor() const noexcept;

                glm::flaot32 getIntensity() const noexcept;

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