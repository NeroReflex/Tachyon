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
						Transfer,
						Present
					};

					typedef std::vector<QueueFamilySupportedOperationType> QueueFamilySelection;

					struct ConcreteQueueFamilyDescriptor {
						QueueFamilySelection supportedOperations;
						uint32_t maxQueues;

						ConcreteQueueFamilyDescriptor(QueueFamilySelection supportedOperations, uint32_t maxQueues) noexcept;
					};

					QueueFamily(Device* const device, const ConcreteQueueFamilyDescriptor& familyDescriptor, uint32_t familyIndex) noexcept;

					QueueFamily(const QueueFamily&) = delete;

					QueueFamily(QueueFamily&&) = delete;

					QueueFamily& operator=(const QueueFamily&) = delete;

					~QueueFamily() override;

					uint32_t getNativeQueueFamilyIndexHandle() const noexcept;

					const std::vector<QueueFamilySupportedOperationType>& getSupportedOperationTypes() const noexcept;

					uint32_t getMaximumQueuesCount() const noexcept;

					Queue* getQueue(uint32_t index) const noexcept;

				private:
					ConcreteQueueFamilyDescriptor mQueueFamilyDescriptor;

					uint32_t mFamilyIndex;

					std::unordered_map<uintptr_t, std::unique_ptr<Queue>> mQueues;
				};
			
			}
		}
	}
}
