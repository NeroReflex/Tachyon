#include "Swapchain.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

Swapchain::Swapchain(const Device* device) noexcept : DeviceOwned(device) {}

Swapchain::~Swapchain() {}

