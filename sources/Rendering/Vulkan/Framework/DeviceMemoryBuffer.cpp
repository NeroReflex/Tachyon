#include "DeviceMemoryBuffer.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DeviceMemoryBuffer::DeviceMemoryBuffer(const Device* device) noexcept : DeviceOwned(device) {}

DeviceMemoryBuffer::~DeviceMemoryBuffer() {}