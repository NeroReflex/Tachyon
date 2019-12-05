#include "DeviceOwned.h"
#include "Device.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

DeviceOwned::DeviceOwned(Device* device) noexcept : mOwningDevice(device) {
	getParentDevice()->mOwnedObjects.emplace(std::pair<uintptr_t, std::unique_ptr<DeviceOwned>>(uintptr_t(this), this));
}

DeviceOwned::~DeviceOwned() {}

Device* DeviceOwned::getParentDevice() const noexcept {
	return mOwningDevice;
}