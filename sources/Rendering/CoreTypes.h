#pragma once

#include "Tachyon.h"

/*
	FOLLOWING DATA STRUCTURES ARE MAINTAINED ALIGNED WITH THE GPU MEMORY
*/

namespace Tachyon {
	namespace Rendering {
		
		struct AABB {
			glm::vec4 position;
			glm::vec4 dimensions;
		};

		struct NodeData {
			AABB aabb;
		};

	}
}