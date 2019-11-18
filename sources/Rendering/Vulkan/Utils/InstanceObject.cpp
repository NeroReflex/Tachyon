#include "InstanceObject.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

InstanceObject::InstanceObject(const std::shared_ptr<Instance>& instance) noexcept
	: mInstance(instance) {}

InstanceObject::~InstanceObject() {}

std::shared_ptr<Instance> InstanceObject::getInstance() const noexcept {
	return mInstance.lock();
}

const std::vector<const char*>& InstanceObject::getInstanceEnabledValidationLayers() const noexcept {
	return getInstance()->mEnabledLayers;
}