#include "SwapchainImage.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

SwapchainImage::SwapchainImage(const Device* device, VkImage&& image) noexcept
	: DeviceOwned(device),
	ImageInterface(device, std::move(image)) {}

SwapchainImage::~SwapchainImage() {}

