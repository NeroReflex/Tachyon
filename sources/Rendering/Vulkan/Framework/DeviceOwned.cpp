#include "DeviceOwned.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DeviceOwned::DeviceOwned(const Device* device) noexcept : mOwningDevice(device) {}

DeviceOwned::~DeviceOwned() {}

const Device* DeviceOwned::getParentDevice() const noexcept {
	return mOwningDevice;
}