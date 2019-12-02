#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Fence :
					virtual public DeviceOwned {

				public:
					Fence(const Device* const device, VkFence&& fence) noexcept;

					Fence(const Fence&) = delete;

					Fence(Fence&&) = delete;

					Fence& operator=(const Fence&) = delete;

					~Fence() override;

				private:
					VkFence mFence;
				};
			}
		}
	}
}
