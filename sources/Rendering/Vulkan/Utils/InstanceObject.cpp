#include "InstanceObject.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Utils;

InstanceObject::InstanceObject(const std::weak_ptr<Instance>& instance) noexcept
	: mInstance(instance) {}

InstanceObject::~InstanceObject() {}

std::shared_ptr<Instance> InstanceObject::getInstance() const noexcept {
	return mInstance;
}
