#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class HDR {
			
			alignas(sizeof(glm::float32)) glm::float32 mGamma;

			alignas(sizeof(glm::float32)) glm::float32 mExposure;

		public:

			constexpr static glm::float32 defaultGamma = 2.2;
			constexpr static glm::float32 defaultExposure = 0.1;

			HDR(glm::float32 gamma = HDR::defaultGamma, glm::float32 exposure = HDR::defaultExposure) noexcept;
		};
	}
}