#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class CommandBuffer;

				class CommandPool :
					virtual public DeviceOwned {
					
					public:
						CommandPool(Device* const, VkCommandPool&& commandPool) noexcept;
						
						CommandPool(const CommandPool&) = delete;
						
						CommandPool(CommandPool&&) = delete;
						
						CommandPool& operator=(const CommandPool&)  = delete;
						
						~CommandPool() override;

						const VkCommandPool& getNativeCommandPoolHandle() const noexcept;

						CommandBuffer* createCommandBuffer() noexcept;
						
					private:
						VkCommandPool mCommandPool;

						std::vector<std::unique_ptr<CommandBuffer>> mRegisteredCommandBuffers;
				};
			}
		}
	}
}
