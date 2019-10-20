#pragma once

#include "ToneMapper.h"

namespace Tachyon {
	namespace Rendering {
		namespace ToneMapping {
			class ReinhardToneMapping :
				virtual public ToneMapper {
			public:
				ReinhardToneMapping() noexcept;

				virtual ~ReinhardToneMapping() = default;

				glm::vec3 operator()(const glm::vec3&) noexcept override;
			};
		}
	}
}