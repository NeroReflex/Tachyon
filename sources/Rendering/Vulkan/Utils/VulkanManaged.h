#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Rendering {
		namespace Vulkan {
			namespace Utils {

				/**
				 * Represents an object that depends on a vulkan instance.
				 */
				class VulkanManaged :
					virtual public std::enable_shared_from_this<VulkanManaged> {

				public:
					VulkanManaged() noexcept;

					VulkanManaged(const VulkanManaged&) = delete;

					VulkanManaged(VulkanManaged&&) = delete;

					VulkanManaged& operator=(const VulkanManaged&) = delete;

					virtual ~VulkanManaged() = default;
				};

			}
		}
	}
}
