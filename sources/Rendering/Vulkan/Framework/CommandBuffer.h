#pragma once

#include "CommandPool.h"
#include "Fence.h"
#include "Queue.h"

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

					void registerCommands(std::function<void(const VkCommandBuffer & commandBuffer)> fn) noexcept;

					void submit(const Queue* const queue, const Fence* const fence) noexcept;

				private:
					VkCommandBuffer mCommandBuffer;

					const CommandPool* const mParentCommandPool;
				};
			}
		}
	}
}