#pragma once

#include "DeviceOwned.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Framework {
				class Queue;

				class QueueFamily :
					virtual public DeviceOwned {

					friend class Queue;

				public:
					enum QueueFamilySupportedOperationType {
						Compute,
						Graphics,
						Present
					};

					QueueFamily(Device* const device, std::vector<QueueFamilySupportedOperationType> supportedOperationTypes, uint32_t familyIndex) noexcept;

					QueueFamily(const QueueFamily&) = delete;

					QueueFamily(QueueFamily&&) = delete;

					QueueFamily& operator=(const QueueFamily&) = delete;

					~QueueFamily() override;

					uint32_t getNativeQueueFamilyIndexHandle() const noexcept;

					const std::vector<QueueFamilySupportedOperationType>& getSupportedOperationTypes() const noexcept;

				private:
					std::vector<QueueFamilySupportedOperationType> mSupportedOperationTypes;

					uint32_t mFamilyIndex;

					std::unordered_map<uintptr_t, std::unique_ptr<Queue>> mQueues;
				};
			
			}
		}
	}
}
