#pragma once

#include "Rendering/RenderingPipeline.h"

#include "Framework/Instance.h"
#include "Framework/Device.h"
#include "Framework/Swapchain.h"
#include "Framework/Shader.h"

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

				Framework::Swapchain* const mSwapchain;

				const Framework::Pipeline* const mInsertPipeline;

				const Framework::Pipeline* const mFlushPipeline;

				const Framework::Pipeline* const mUpdatePipeline;

				const Framework::Pipeline* const mRenderingPipeline;

				Framework::Image* const mRaytracingTLAS;
				Framework::ImageView* mRaytracingTLASImageView;

				Framework::Image* const mRaytracingBLASCollection;
				Framework::ImageView* mRaytracingBLASCollectionImageView;

				Framework::Image* const mRaytracingModelMatrix;
				Framework::ImageView* mRaytracingModelMatrixImageView;

				Framework::Image* const mRaytracingGeometryCollection;
				Framework::ImageView* mRaytracingGeometryCollectionImageView;

				Framework::DescriptorPool* mRaytracerDescriptorPool;

				Framework::DescriptorSet* mRaytracerInsertDescriptorSet;

				Framework::DescriptorSet* mRaytracerFlushDescriptorSet;

				Framework::DescriptorSet* mRaytracerUpdateDescriptorSet;

				Framework::DescriptorSet* mRaytracerRenderingDescriptorSet;

				Framework::CommandPool* const mRaytracerCommandPool;

				Framework::CommandBuffer* const mRaytracerFlushCommandBuffer;
			};
		}
    }
}
