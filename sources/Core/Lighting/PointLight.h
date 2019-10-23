#pragma once

#include "Core/Lighting/Light.h"

namespace Tachyon {
	namespace Core {
        namespace Lighting {
		    class PointLight :
                public Light {
                    
            public:
                PointLight(const glm::vec3& color, const glm::float32& intensity, const glm::vec3& position) noexcept;

				glm::vec3 getPosition() const noexcept;

            private:
                glm::vec4 mPosition;
            };
        }
	}
}