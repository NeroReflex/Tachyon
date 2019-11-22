#include "VulkanPipeline.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;

#include "shaders/vulkan/raytrace_insert.comp.spv.h" // raytrace_insert_compVK, raytrace_insert_compVK_size
#include "shaders/vulkan/raytrace_flush.comp.spv.h" // raytrace_flush_compVK, raytrace_flush_compVK_size
#include "shaders/vulkan/raytrace_render.comp.spv.h" // raytrace_render_compVK, raytrace_render_compVK_size
#include "shaders/vulkan/raytrace_update.comp.spv.h" // raytrace_update_compVK, raytrace_update_compVK_size

VulkanPipeline::VulkanPipeline(GLFWwindow* window) noexcept
	: RenderingPipeline(window),
	mInstance(new Framework::Instance(getGLFWwindow())),
	mDevice(mInstance->openDevice()),
	mGeometryInsertShader(
		mDevice,
		Framework::Shader::ShaderType::Compute,
		Framework::ShaderLayoutBinding({}),
		reinterpret_cast<const char*>(raytrace_insert_compVK),
		raytrace_insert_compVK_size) {

#if defined(VULKAN_ENABLE_VALIDATION_LAYERS) 
	std::cout << "Available Vulkan extensions:" << std::endl;

	for (const auto& extension : Framework::Instance::getAllSupportedExtensions()) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}
#endif

	mDevice->createSwapchain(getWidth(), getHeight());
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