#pragma once

#include "DeviceOwned.h"
#include "ShaderLayoutBinding.h"
#include "DescriptorSet.h"
#include "Pipeline.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class DescriptorPool :
					virtual public DeviceOwned {

					friend class DescriptorSet;

				public:
					DescriptorPool(Device* const device, VkDescriptorPool&& descriptorPool) noexcept;

					DescriptorPool(const DescriptorPool&) = delete;

					DescriptorPool(DescriptorPool&&) = delete;

					DescriptorPool& operator=(const DescriptorPool&) = delete;

					~DescriptorPool() override;

					const VkDescriptorPool& getNativeDescriptorPoolHandle() const noexcept;

					DescriptorSet* allocateDescriptorSet(const Pipeline* pipeline) noexcept;

				private:
					VkDescriptorPool mDescriptorPool;

					std::unordered_map<uintptr_t, std::unique_ptr<DescriptorSet>> mAllocatedDescriptorSets;
				};
			}
		}
	}
}
