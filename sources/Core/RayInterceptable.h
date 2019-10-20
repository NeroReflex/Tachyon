#pragma once

#include "Ray.h"

namespace Tachyon {
	namespace Core {
		class RayInterceptable {
		public:
			virtual bool hit(const Ray& ray) const noexcept = 0;
		};
	}
}