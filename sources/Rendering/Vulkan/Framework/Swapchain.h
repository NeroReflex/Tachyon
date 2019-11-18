#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Swapchain :
					virtual public DeviceOwned {

				public:
					Swapchain(const Device* device) noexcept;

					Swapchain(Swapchain&) = delete;

					Swapchain& operator=(Swapchain&) = delete;

					~Swapchain() override;

				private:

					
				};

			}
		}
	}
}
