#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {
				
				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class InstanceObject {

				public:
					InstanceObject(const std::weak_ptr<Instance>& instance) noexcept;

					InstanceObject(const InstanceObject&) = delete;

					InstanceObject(InstanceObject&&) = delete;

					InstanceObject& operator=(const InstanceObject&) = delete;

					virtual ~InstanceObject();

					std::shared_ptr<Instance> getInstance() const noexcept;

				private:
					std::weak_ptr<Instance> mInstance;
				};

			}
		}
	}
}