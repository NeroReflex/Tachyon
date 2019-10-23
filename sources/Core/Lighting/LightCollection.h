#pragma once

#include "Core/Lighting/Light.h"
#include "Core/Collection.hpp"

namespace Tachyon {
	namespace Core {
		namespace Lighting {
			class LightCollection :
				public Collection<Light, 4> {

			public:

				// Use collection constructors
				using Collection::Collection;
			};
		}
	}
}