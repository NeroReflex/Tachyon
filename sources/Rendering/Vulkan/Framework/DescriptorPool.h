#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class DescriptorPool :
					virtual public DeviceOwned {

				public:
					DescriptorPool(const Device* const device, VkDescriptorPool&& descriptorPool) noexcept;

					DescriptorPool(const DescriptorPool&) = delete;

					DescriptorPool(DescriptorPool&&) = delete;

					DescriptorPool& operator=(const DescriptorPool&) = delete;

					~DescriptorPool() override;

					const VkDescriptorPool& getNativeDescriptorPoolHandle() const noexcept;

				private:
					VkDescriptorPool mDescriptorPool;
				};
			}
		}
	}
}
