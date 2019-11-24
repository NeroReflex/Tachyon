#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class HDR {
			
			alignas(sizeof(glm::float32)) glm::float32 mGamma;

			alignas(sizeof(glm::float32)) glm::float32 mExposure;

		public:

			static const glm::float32 defaultGamma;
			static const glm::float32 defaultExposure;

			HDR(glm::float32 gamma = HDR::defaultGamma, glm::float32 exposure = HDR::defaultExposure) noexcept;
		};
	}
}