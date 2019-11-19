#pragma once

#include "Rendering/RenderingPipeline.h"

#include "Framework/Instance.h"
#include "Framework/Device.h"
#include "Framework/Swapchain.h"

namespace Tachyon {
    namespace Rendering {
		namespace Vulkan {
			class VulkanPipeline final :
				virtual public Rendering::RenderingPipeline {
				
			public:
				VulkanPipeline(GLFWwindow* window) noexcept;

				VulkanPipeline(const VulkanPipeline&) = delete;

				VulkanPipeline(VulkanPipeline&&) = delete;

				VulkanPipeline& operator=(const VulkanPipeline&) = delete;

				~VulkanPipeline() override;

				void enqueueModel(std::vector<GeometryPrimitive>&& primitive, const GeometryInsertAttributes& insertData) noexcept final;
				
			protected:
				void onReset() noexcept final;

				void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept final;

				void onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept final;

			private:
				std::unique_ptr<Framework::Instance> mInstance;

				Framework::Device* mDevice;
			};
		}
    }
}
