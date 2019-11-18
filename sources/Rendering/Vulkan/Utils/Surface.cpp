#include "Surface.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

Surface::Surface(const std::shared_ptr<Instance>& instance, VkSurfaceKHR surface) noexcept
	: InstanceObject(instance),
	mSurface(std::move(surface)) {

}

Surface::~Surface() {
	vkDestroySurfaceKHR(getInstance()->getNativeHandle(), mSurface, nullptr);
}

const VkSurfaceKHR& Surface::getNativeHandle() const noexcept {
	return mSurface;
}
