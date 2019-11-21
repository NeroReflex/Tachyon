#include "Image.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Image::Image(const Device* device, VkImage&& image) noexcept
	: DeviceOwned(device),
	ImageInterface(device, std::move(image)) {}

    
Image::~Image() {}
