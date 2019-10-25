#pragma once

#include "ToneMapper.h"

namespace Tachyon {
	namespace Rendering {
		namespace CPU {
			namespace ToneMapping {

				class ExposureToneMapper :
					virtual public ToneMapper {
				public:
					ExposureToneMapper(glm::float32 exposure = 0.1) noexcept;

					virtual ~ExposureToneMapper() = default;

					glm::vec3 operator()(const glm::vec3&) noexcept override;

				private:
					glm::float32 mExposure;
				};

			}
		}
	}
}