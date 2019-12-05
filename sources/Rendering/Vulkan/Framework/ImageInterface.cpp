#include "ImageInterface.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

ImageInterface::ImageInterface(Device* device, VkImage&& image) noexcept
	: DeviceOwned(device),
	mImage(std::move(image)) {}

ImageInterface::~ImageInterface() {}

const VkImage& ImageInterface::getNativeImageHandle() const noexcept {
	return mImage;
}
