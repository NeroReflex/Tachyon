#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Device;

				class DeviceOwned {

				public:
					DeviceOwned(Device* device) noexcept;

					DeviceOwned(DeviceOwned&) = delete;

					DeviceOwned& operator=(DeviceOwned&) = delete;

					virtual ~DeviceOwned();

					Device* getParentDevice() const noexcept;

				private:
					Device* mOwningDevice;
				};

			}
		}
	}
}
