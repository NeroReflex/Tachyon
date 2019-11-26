#include "UnsafePoolManager.h"

using namespace Tachyon;
using namespace Tachyon::Memory;

UnsafePoolManager::UnsafePoolManager(UnsafePoolManager::managementType blockCount, void* const basePtr, managementType* managementStructure) noexcept 
	: mBasePtr(basePtr),
	mMemoryMap(managementStructure),
	mBlockCount(blockCount) {
	DBG_ASSERT(((uintptr_t(mBasePtr) % atomicMemoryBlockSize) == 0));
	
	// Initialize memory as free blocks
	for (index_t i = 0; i < mBlockCount; ++i) mMemoryMap[i] = 0;
}

UnsafePoolManager::UnsafePoolManager(UnsafePoolManager&& src) noexcept
	: mBasePtr(src.mBasePtr),
	mMemoryMap(src.mMemoryMap),
	mBlockCount(src.mBlockCount) {
	src.mBasePtr = nullptr;
	src.mMemoryMap = nullptr;
	src.mBlockCount = 0;
}

UnsafePoolManager::UnsafePoolManager(const UnsafePoolManager& src) noexcept
	: mBasePtr(src.mBasePtr),
	mMemoryMap(src.mMemoryMap),
	mBlockCount(src.mBlockCount) {}
	
UnsafePoolManager& UnsafePoolManager::operator=(const UnsafePoolManager& src) noexcept {
	if (this != &src) {
		mBasePtr = src.mBasePtr;
		mMemoryMap = src.mMemoryMap;
		mBlockCount = src.mBlockCount;
	}
	
	return *this;
}

UnsafePoolManager::AllocResult UnsafePoolManager::malloc(size_t bytesSize, size_t bytesAlign) noexcept {
	index_t requiredBlocks = queryRequiredBlockCount(bytesSize, bytesAlign);
	
	index_t increment =  (((bytesAlign % atomicMemoryBlockSize) > 0) ? 1 : 0) + (bytesAlign / atomicMemoryBlockSize);
	
	void* base = mBasePtr;
	std::size_t globalSizeInBytes = atomicMemoryBlockSize * mBlockCount;
	index_t i = indexFromAddress(std::align(bytesAlign, bytesSize, base, globalSizeInBytes));
	
	do {
		index_t consecutiveFreeBlocks = 0;
		
		do { // Search for requiredBlocks consecutive free blocks
			consecutiveFreeBlocks = (mMemoryMap[i + consecutiveFreeBlocks] == 0) ? consecutiveFreeBlocks+1 : 0;
		} while ((consecutiveFreeBlocks < requiredBlocks) && (consecutiveFreeBlocks > 0));
		
		if (consecutiveFreeBlocks == requiredBlocks) { // Mark memory as used
			for (index_t m = 0; m < requiredBlocks; ++m) mMemoryMap[i + m] = 1;
			
			void* allocatedBase = addressFromIndex(i);
			std::size_t available = atomicMemoryBlockSize * (requiredBlocks + 1);
			void* aligned = std::align(bytesAlign, bytesSize, allocatedBase, available);
			return UnsafePoolManager::AllocResult(true, aligned);
		}
		
		i += increment;
	} while(i < mBlockCount);
	
	return UnsafePoolManager::AllocResult(false, nullptr);
}

void UnsafePoolManager::free(void* memory, size_t bytesSize, size_t bytesAlign) noexcept {
	index_t requiredBlocks = queryRequiredBlockCount(bytesSize, bytesAlign);
	
	index_t i = index_t(memory);
	
	for (index_t m = 0; m < requiredBlocks; ++m) mMemoryMap[i + m] = 0;
}
