#pragma once

#include "Instance.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {
				
				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class InstanceObject :
					virtual public VulkanManaged {

				public:
					InstanceObject(const std::shared_ptr<Instance>& instance) noexcept;

					InstanceObject(const InstanceObject&) = delete;

					InstanceObject(InstanceObject&&) = delete;

					InstanceObject& operator=(const InstanceObject&) = delete;

					virtual ~InstanceObject();

					std::shared_ptr<Instance> getInstance() const noexcept;

				protected:
					const std::vector<const char*>& getInstanceEnabledValidationLayers() const noexcept;

				private:
					std::weak_ptr<Instance> mInstance;
				};

			}
		}
	}
}