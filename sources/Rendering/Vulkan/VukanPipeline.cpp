#include "VulkanPipeline.h"

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
	mDevice(mInstance->openDevice()),
	mSwapchain(mDevice->createSwapchain(getWidth(), getHeight())),
	mGeometryInsertShader(
		mDevice->loadComputeShader(
			Framework::ShaderLayoutBinding({
				// Core bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, TLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, GEOMETRY_BINDING,  1),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageImage, BLAS_ATTRIBUTES_BINDING,  1),

				// Insert-specific bindings
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::StorageBuffer, GEOMETRY_INSERT_BINDING,  1 ),
				Framework::ShaderLayoutBinding::BindingDescriptor(Framework::ShaderLayoutBinding::BindingType::UniformBuffer, GEOMETRY_INSERTT_ATTR_BINDING,  1 )
			}),
			reinterpret_cast<const char*>(raytrace_insert_compVK),
			raytrace_insert_compVK_size
		)
	),
	mInsertPipeline(mDevice->createPipeline(std::vector<const Framework::Shader*>({ mGeometryInsertShader }))) {

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	std::cout << "Available Vulkan extensions:" << std::endl;
	
	for (const auto& extension : Framework::Instance::getAllSupportedExtensions())
		std::cout << "\t" << extension.extensionName << std::endl;
#endif
}

VulkanPipeline::~VulkanPipeline() {
	
}

void VulkanPipeline::enqueueModel(std::vector<GeometryPrimitive>&& primitive, const GeometryInsertAttributes& insertData) noexcept {

}

void VulkanPipeline::onReset() noexcept {

}

void VulkanPipeline::onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept {

}

void VulkanPipeline::onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept {

}
