#include "Allocator.hpp"

using namespace Tachyon;
using namespace Tachyon::Memory;

Allocator<void>::Allocator(size_t pagesCount) noexcept
	: mMemPool(calloc((pagesCount* numberOfBlocksInPage) + 1, atomicMemoryBlockSize)), mManager(pagesCount, blockAlign(mMemPool)) {}

Allocator<void>::Allocator(const Allocator<void>& src) noexcept
	: mMemPool(src.mMemPool), mManager(src.mManager.getPagesCount(), blockAlign(mMemPool), reinterpret_cast<uint32_t*>(blockAlign(mMemPool)), false) {}

Allocator<void>::~Allocator() {
	free(mMemPool);
}

Allocator<void>::pointer Allocator<void>::allocate(Allocator<void>::size_type n, const void* hint) noexcept {
	auto allocResult = mManager.malloc(n, 1, hint);

	DBG_ASSERT(allocResult.success);

	return allocResult.result;
}

void Allocator<void>::deallocate(Allocator<void>::pointer p, size_type n) noexcept {
	mManager.free(p, n);
}

Allocator<void>::size_type Allocator<void>::max_size() const noexcept {
	return mManager.getMaxAllocationSize();
}