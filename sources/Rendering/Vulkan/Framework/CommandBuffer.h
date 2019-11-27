#pragma once

#include "CommandPool.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class CommandBuffer {

				public:
					CommandBuffer(const CommandPool* const parentCommandPool, VkCommandBuffer&& commandBuffer) noexcept;

					CommandBuffer(const CommandBuffer&) = delete;

					CommandBuffer(CommandBuffer&&) = delete;

					CommandBuffer& operator=(const CommandBuffer&) = delete;

					~CommandBuffer();

				private:
					VkCommandBuffer mCommandBuffer;

					const CommandPool* const mParentCommandPool;
				};
			}
		}
	}
}