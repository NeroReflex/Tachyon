#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace ToneMapping {
			class ToneMapper {
			public:
				virtual glm::vec3 operator()(const glm::vec3&) noexcept = 0;

				virtual ~ToneMapper() = default;
			};
		}
	}
}
