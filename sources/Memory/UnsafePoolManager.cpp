#include "UnsafePoolManager.h"

using namespace Tachyon;
using namespace Tachyon::Memory;

UnsafePoolManager::UnsafePoolManager(std::size_t blockCount, void* const basePtr, void* const managementStructure) noexcept
	: mBasePtr(basePtr),
	mMemoryMap(reinterpret_cast<managementType*>((managementStructure != nullptr) ? managementStructure : basePtr)),
	mBlockCount(std::move(blockCount)) {
	
	DBG_ASSERT((mBlockCount >= 1));

	// The management structure is carved from the memory pool
	if (reinterpret_cast<void*>(mMemoryMap) == mBasePtr) {
		const auto reservedSpace = getManagementReservedSpace(mBlockCount);
		size_t reservedPageCount = (reservedSpace / atomicMemoryBlockSize) + (((reservedSpace % atomicMemoryBlockSize) == 0) ? 0 : 1);

		DBG_ASSERT((mBlockCount > reservedPageCount));

		mBlockCount -= reservedPageCount;
		mBasePtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mBasePtr) + (reservedPageCount * atomicMemoryBlockSize));
	}


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
	
UnsafePoolManager::~UnsafePoolManager() {}

UnsafePoolManager& UnsafePoolManager::operator=(const UnsafePoolManager& src) noexcept {
	if (this != &src) {
		mBasePtr = src.mBasePtr;
		mMemoryMap = src.mMemoryMap;
		mBlockCount = src.mBlockCount;
	}
	
	return *this;
}

bool UnsafePoolManager::operator==(const UnsafePoolManager& cmp) const noexcept {
	return this->mBasePtr == &(cmp.mBasePtr);
}

UnsafePoolManager::AllocResult UnsafePoolManager::malloc(size_t bytesSize, size_t bytesAlign) noexcept {
	index_t requiredBlocks = queryRequiredBlockCount(bytesSize, bytesAlign);
	
	index_t increment =  (((bytesAlign % atomicMemoryBlockSize) > 0) ? 1 : 0) + (bytesAlign / atomicMemoryBlockSize);
	
	void* base = mBasePtr;
	std::size_t globalSizeInBytes = atomicMemoryBlockSize * mBlockCount;
	index_t i = indexFromAddress(std::align(bytesAlign, bytesSize, base, globalSizeInBytes));
	
	do {
		index_t consecutiveFreeBlocks = 0;
		
		if ((i + requiredBlocks) < mBlockCount) {
			do { // Search for requiredBlocks consecutive free blocks
				consecutiveFreeBlocks = (mMemoryMap[i + consecutiveFreeBlocks] == 0) ? consecutiveFreeBlocks+1 : 0;
			} while ((consecutiveFreeBlocks < requiredBlocks) && (consecutiveFreeBlocks > 0));
		}
		
		if (consecutiveFreeBlocks == requiredBlocks) { // Mark memory as used
			for (index_t m = 0; m < requiredBlocks; ++m) mMemoryMap[i + m] = 1;
			
			void* allocatedBase = addressFromIndex(i);
			std::size_t available = atomicMemoryBlockSize * (requiredBlocks + 1);
			void* aligned = std::align(bytesAlign, bytesSize, allocatedBase, available);

			DBG_ASSERT( ((uintptr_t(aligned) % bytesAlign) == 0) ); // Check alignment

			return UnsafePoolManager::AllocResult(true, aligned);
		}
		
		i += increment;
	} while(i < mBlockCount);
	
	return UnsafePoolManager::AllocResult(false, nullptr);
}

void UnsafePoolManager::free(void* memory, size_t bytesSize, size_t bytesAlign) noexcept {
	index_t requiredBlocks = queryRequiredBlockCount(bytesSize, bytesAlign);
	
	index_t i = indexFromAddress(memory);
	
	for (index_t m = 0; m < requiredBlocks; ++m) mMemoryMap[i + m] = 0;
}
