#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				/**
				 * This class acts as an interface for physical memory allocated.
				 */
				class DeviceMemoryBuffer :
					virtual public DeviceOwned {

				public:
					DeviceMemoryBuffer(const Device* device) noexcept;

					DeviceMemoryBuffer(const DeviceMemoryBuffer&) = delete;

					DeviceMemoryBuffer(DeviceMemoryBuffer&&) = delete;

					DeviceMemoryBuffer& operator=(const DeviceMemoryBuffer&) = delete;

					~DeviceMemoryBuffer() override;
				};

			}
		}
	}
}
