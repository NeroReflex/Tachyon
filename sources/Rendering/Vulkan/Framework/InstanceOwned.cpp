#include "InstanceOwned.h"

using namespace Tachyon;
using namespace Tachyon::Rendering;
using namespace Tachyon::Rendering::Vulkan;
using namespace Tachyon::Rendering::Vulkan::Framework;

InstanceOwned::InstanceOwned(const Instance* instance) noexcept : mOwningInstance(instance) {}

InstanceOwned::~InstanceOwned() {}

