#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Device;

				class DeviceOwned {

				public:
					DeviceOwned(const Device* device) noexcept;

					DeviceOwned(DeviceOwned&) = delete;

					DeviceOwned& operator=(DeviceOwned&) = delete;

					virtual ~DeviceOwned();

					const Device* getParentDevice() const noexcept;

				private:
					const Device* mOwningDevice;
				};

			}
		}
	}
}
