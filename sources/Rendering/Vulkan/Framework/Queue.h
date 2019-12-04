#pragma once

#include "QueueFamily.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {

				class Queue  {

				public:
					Queue(QueueFamily* const queueFamily, VkQueue&& queue) noexcept;

					~Queue();

					QueueFamily* getParentQueueFamily() const noexcept;

					const VkQueue& getNativeQueueHandle() const noexcept;

				private:
					QueueFamily* const mQueueFamily;

					VkQueue mQueue;
				};

			}
		}
	}
}
