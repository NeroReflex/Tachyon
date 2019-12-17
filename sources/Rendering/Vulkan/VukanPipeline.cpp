#include "VulkanPipeline.h"

#include "Framework/CommandBuffer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;

#include "shaders/config.glsl"
#include "shaders/vulkan/raytrace_insert.comp.spv.h" // raytrace_insert_compVK, raytrace_insert_compVK_size
#include "shaders/vulkan/raytrace_flush.comp.spv.h" // raytrace_flush_compVK, raytrace_flush_compVK_size
#include "shaders/vulkan/raytrace_render.comp.spv.h" // raytrace_render_compVK, raytrace_render_compVK_size
#include "shaders/vulkan/raytrace_update.comp.spv.h" // raytrace_update_compVK, raytrace_update_compVK_size

VulkanPipeline::VulkanPipeline(GLFWwindow* window) noexcept
	: RenderingPipeline(window),
	mInstance(new Framework::Instance(getGLFWwindow())),
	mDevice(mInstance->openDevice({ 
		Framework::QueueFamily::ConcreteQueueFamilyDescriptor({
			Framework::QueueFamily::QueueFamilySupportedOperationType::Transfer,
			Framework::QueueFamily::QueueFamilySupportedOperationType::Compute,
			Framework::QueueFamily::QueueFamilySupportedOperationType::Graphics,
			Framework::QueueFamily::QueueFamilySupportedOperationType::Present
		}, 1)
	})),
	mQueueFamily(mDevice->getQueueFamily(0)),
	mQueue(mQueueFamily->getQueue(0)),
	mSwapchain(mDevice->createSwapchain({ mQueueFamily }, getWidth(), getHeight())),
	mInsertPipeline(mDevice->createPipeline(std::vector<const Framework::Shader*>({
		mDevice->loadComputeShader(
			Framework::ShaderLayoutBinding({
				// Insert-specific bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageBuffer, GEOMETRY_INSERT_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::UniformBuffer, GEOMETRY_INSERTT_ATTR_BINDING,  1),

				// Core bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, TLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, GEOMETRY_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_ATTRIBUTES_BINDING,  1)
			}),
			reinterpret_cast<const char*>(raytrace_insert_compVK),
			raytrace_insert_compVK_size
		)
		}))),
	mFlushPipeline(mDevice->createPipeline(std::vector<const Framework::Shader*>({
		mDevice->loadComputeShader(
			Framework::ShaderLayoutBinding({
				// Core bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, TLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, GEOMETRY_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_ATTRIBUTES_BINDING,  1),
			}),
			reinterpret_cast<const char*>(raytrace_flush_compVK),
			raytrace_flush_compVK_size
		)
		}))),
	mUpdatePipeline(mDevice->createPipeline(std::vector<const Framework::Shader*>({
		mDevice->loadComputeShader(
			Framework::ShaderLayoutBinding({
				// Core bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, TLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, GEOMETRY_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_ATTRIBUTES_BINDING,  1),
			}),
			reinterpret_cast<const char*>(raytrace_update_compVK),
			raytrace_update_compVK_size
		)
		}))),
	mRenderingPipeline(mDevice->createPipeline(std::vector<const Framework::Shader*>({
		mDevice->loadComputeShader(
			Framework::ShaderLayoutBinding({
				// Debug binding
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, DEBUG_BINDING,  1),

				// Render target binding
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, OUTPUT_BINDING,  1),

				// HDR data binding
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::UniformBuffer, HDR_BINDING,  1),

				// Camera data binding
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::UniformBuffer, CAMERA_BINDING,  1),

				// Core bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, TLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, GEOMETRY_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_ATTRIBUTES_BINDING,  1),
			}),
			reinterpret_cast<const char*>(raytrace_render_compVK),
			raytrace_render_compVK_size
		)
		}))),
	mRaytracingTLAS(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, (2 * 16) * (1 << expOfTwo_maxModels) )), // 2*16 is sizeof(AABB)
	mRaytracingBLASCollection(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, (2 * 16) * (1 << expOfTwo_maxModels) * numberOfTreeElementsToContainExpOfTwoLeafs(expOfTwo_maxCollectionsForModel) )),
	mRaytracingModelMatrix(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(glm::mat4) * (1 << expOfTwo_maxModels) )),
	mRaytracingGeometryCollection(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(Core::Triangle)* (1 << expOfTwo_maxModels) * (1 << expOfTwo_maxCollectionsForModel) * (1 << expOfTwo_maxGeometryOnCollection) )),
	
	// Buffer used on model insert
	mRaytracerInsertModelAttributesUniformBuffer(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GeometryInsertAttributes))),
	mRaytracerInsertModelGeometryStorageBuffer(mDevice->createBuffer({ mQueueFamily }, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
		sizeof(glm::vec4)* (size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryCollectionOnBLAS)* size_t(size_t(1) << mRaytracerInfo.expOfTwo_numberOfGeometryOnCollection)
	)),

	// Descriptor pool and descriptor set for each shader
	mRaytracerDescriptorPool(mDevice->createDescriptorPool({
		std::make_tuple(Framework::ShaderLayoutBinding::BindingType::StorageBuffer, uint32_t(1)),
		std::make_tuple(Framework::ShaderLayoutBinding::BindingType::UniformBuffer, uint32_t(3)),
		std::make_tuple(Framework::ShaderLayoutBinding::BindingType::StorageImage, uint32_t(18)),
	}, 4)),
	mRaytracerInsertDescriptorSet(mRaytracerDescriptorPool->allocateDescriptorSet(mInsertPipeline)),
	mRaytracerFlushDescriptorSet(mRaytracerDescriptorPool->allocateDescriptorSet(mFlushPipeline)),
	mRaytracerUpdateDescriptorSet(mRaytracerDescriptorPool->allocateDescriptorSet(mUpdatePipeline)),
	mRaytracerRenderingDescriptorSet(mRaytracerDescriptorPool->allocateDescriptorSet(mRenderingPipeline)),
	mRaytracerCommandPool(mDevice->createCommandPool({ mQueueFamily })),
	mRaytracerFlushCommandBuffer(mRaytracerCommandPool->createCommandBuffer()),
	mRaytracerInsertCommandBuffer(mRaytracerCommandPool->createCommandBuffer()),
	
	// Command fences
	mRaytracerFlushFence(mDevice->createFence()),
	mRaytracerInsertFence(mDevice->createFence())
{
	// Allocate memory for core buffers on the GPU exclusive memory.
	mDevice->allocateResources(
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		{
			mRaytracingTLAS,
			mRaytracingBLASCollection,
			mRaytracingModelMatrix,
			mRaytracingGeometryCollection
		}
	);

	// Allocate memory for buffers that are used on medel insert (and populated by the CPU).
	mDevice->allocateResources(
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		{
			mRaytracerInsertModelAttributesUniformBuffer,
			mRaytracerInsertModelGeometryStorageBuffer,
		}
	);

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	std::cout << "Available Vulkan extensions:" << std::endl;
	
	for (const auto& extension : Framework::Instance::getAllSupportedExtensions())
		std::cout << "\t" << extension.extensionName << std::endl;
#endif
}

VulkanPipeline::~VulkanPipeline() {
	
}

void VulkanPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitive, const GeometryInsertAttributes& insertData) noexcept {
	// perform the update of the descriptor set.
	mRaytracerInsertDescriptorSet->bindStorageBuffers(TLAS_BINDING, {
		mRaytracingTLAS,
		mRaytracingBLASCollection,
		mRaytracingModelMatrix,
		mRaytracingGeometryCollection,
	});
	
	/*
	mRaytracerInsertDescriptorSet->bindUniformBuffers(GEOMETRY_INSERTT_ATTR_BINDING, { mRaytracerInsertModelAttributesUniformBuffer });
	mRaytracerInsertDescriptorSet->bindStorageBuffers(GEOMETRY_INSERT_BINDING, { mRaytracerInsertModelGeometryStorageBuffer });
	
	mRaytracerInsertCommandBuffer->registerCommands([this](const VkCommandBuffer& commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mInsertPipeline->getNativePipelineHandle());
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mInsertPipeline->getNativePipelineLayoutHandle(), 0, 1, &(mRaytracerInsertDescriptorSet->getNativeDescriptorSetHandle()), 0, NULL);

		vkCmdDispatch(commandBuffer, (uint32_t)ceil(static_cast<float>(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels) / float(FLUSH_WORKGROUP_X)), 1, 1);
	});

	mRaytracerInsertCommandBuffer->submit(mQueue, mRaytracerFlushFence);
	*/
}

void VulkanPipeline::onReset() noexcept {
	// perform the update of the descriptor set.
	mRaytracerFlushDescriptorSet->bindStorageBuffers(TLAS_BINDING, {
		mRaytracingTLAS,
		mRaytracingBLASCollection,
		mRaytracingModelMatrix,
		mRaytracingGeometryCollection,
	});

	mRaytracerFlushCommandBuffer->registerCommands([this](const VkCommandBuffer& commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mFlushPipeline->getNativePipelineHandle());
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mFlushPipeline->getNativePipelineLayoutHandle(), 0, 1, &(mRaytracerFlushDescriptorSet->getNativeDescriptorSetHandle()), 0, NULL);

		vkCmdDispatch(commandBuffer, (uint32_t)ceil(static_cast<float>(size_t(1) << mRaytracerInfo.expOfTwo_numberOfModels) / float(FLUSH_WORKGROUP_X)), 1, 1);
	});

	mRaytracerFlushCommandBuffer->submit(mQueue, mRaytracerFlushFence);
}

void VulkanPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {

}

void VulkanPipeline::onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept {

}
