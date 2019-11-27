#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				
				class CommandPool :
					virtual public DeviceOwned {
					
					public:
						CommandPool(const Device* const, VkCommandPool&& commandPool) noexcept;
						
						CommandPool(const CommandPool&) = delete;
						
						CommandPool(CommandPool&&) = delete;
						
						CommandPool& operator=(const CommandPool&)  = delete;
						
						void registerCommands(std::function<void()> fn) noexcept;
						
						~CommandPool() override;
						
					private:
						VkCommandPool mCommandPool;
						
				};
			}
		}
	}
}
