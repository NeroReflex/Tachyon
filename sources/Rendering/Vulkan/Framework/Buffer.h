#pragma once

#include "DeviceOwned.h"
#include "SpaceRequiringResource.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Buffer :
					virtual public DeviceOwned,
					virtual public SpaceRequiringResource {

				public:
					Buffer(Device* const device, VkBuffer&& buffer) noexcept;

					Buffer(const Buffer&) = delete;

					Buffer(Buffer&&) = delete;

					Buffer& operator=(const Buffer&) = delete;

					~Buffer();

					const VkBuffer& getNativeBufferHandle() const noexcept;

				protected:
					std::unique_ptr<VkMemoryRequirements> queryMemoryRequirements() const noexcept final;

					void bindMemory(const Device* const device, VkDeviceMemory memoryPool, VkDeviceSize offset) const noexcept final;

				private:
					VkBuffer mBuffer;
				};
			}
		}
	}
}
