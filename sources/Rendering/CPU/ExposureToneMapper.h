#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace CPU {

			class ExposureToneMapper {
			public:
				ExposureToneMapper(glm::float32 gamma, glm::float32 exposure = 0.1) noexcept;

				virtual ~ExposureToneMapper() = default;

				glm::vec3 operator()(const glm::vec3&) const noexcept;

			private:
				glm::float32 mGamma, mExposure;
			};

		}
	}
}
