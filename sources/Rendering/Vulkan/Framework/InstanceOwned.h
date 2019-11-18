#pragma once

#include "Instance.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class InstanceOwned {

				public:
					InstanceOwned(const Instance* instance) noexcept;

					InstanceOwned(InstanceOwned&) = delete;

					InstanceOwned& operator=(InstanceOwned&) = delete;

					virtual ~InstanceOwned();

				private:
					const Instance* mOwningInstance;
				};

			}
		}
	}
}
