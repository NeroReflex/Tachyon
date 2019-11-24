#include "PoolManager.h"

using namespace Tachyon;
using namespace Tachyon::Memory;

PoolManager::PoolManager(size_t pagesCount, void* const basePtr, uint32_t* const fixedMem, bool reset) noexcept
	: mBasePtr(basePtr),
	mMemoryMap((fixedMem == nullptr) ? reinterpret_cast<uint32_t*>(basePtr) : fixedMem),
	mPagesCount(std::move(pagesCount)) {

		static_assert(((atomicMemoryPageSize / atomicMemoryBlockSize) == 32), "An atomic page size must be 32-times the size of an atomic block");

		// Memory pool to be managed must begin on a memory address multiple of atomicMemoryBlockSize (blocks MUST be atomicMemoryBlockSize-aligned)
		DBG_ASSERT(((uintptr_t(basePtr) % atomicMemoryBlockSize) == 0));

		if (reset) {
			const auto managementReserverCount = getNumberOfPagesUsedToManagePages(pagesCount);
			for (size_t i = 0; i < mPagesCount; ++i)
				mMemoryMap[i] = (i < managementReserverCount) ? static_cast<uint32_t>(0x00000000) : static_cast<uint32_t>(0xFFFFFFFF);
		}
}

PoolManager::~PoolManager() {
	// I could check for the end of memory allocations, but that would make the poolmanager useless in debug build when used to support
	// an alternative of std::allocator
}

void* PoolManager::addrFromBlockIdentifier(const PoolManager::blockIdentifier& blk) const noexcept {
	return reinterpret_cast<void*>(uintptr_t(mBasePtr) + ((uintptr_t((blk.pageId * shiftLimit) + blk.blockId)) * atomicMemoryBlockSize));
}

PoolManager::blockIdentifier PoolManager::blockIdentifierFromAddr(const void* addr) const noexcept {
	uintptr_t offset = uintptr_t(addr) - uintptr_t(mBasePtr);
	uintptr_t blockSequentialNumber = offset / atomicMemoryBlockSize;

	size_t pageNumber = blockSequentialNumber / shiftLimit;
	size_t blockNumber = blockSequentialNumber % shiftLimit;

	return blockIdentifier(pageNumber, blockNumber);
}

PoolManager::AllocResult PoolManager::malloc(size_t bytes, size_t bytesAlign, const void* const hint) noexcept {
	DBG_ASSERT(bytesAlign < atomicMemoryBlockSize);

	size_t baseAlignment = atomicMemoryBlockSize;
	size_t memoryRequiredForAlignmentInBytes = bytesAlign % atomicMemoryBlockSize;

	size_t requiredMemoryBlocks = countRequiredMemoryBlock(bytes) + ((memoryRequiredForAlignmentInBytes > 0) ? countRequiredMemoryBlock(memoryRequiredForAlignmentInBytes) : 0);

	const auto freeSearchResult = freeSearch(requiredMemoryBlocks, hint);
	bool success = freeSearchResult.pageId < mPagesCount;

	if (!success)
		return AllocResult(nullptr, success);

	size_t markedBlockAsUsed = 0;
	blockIdentifier blockToBeMarkedAsUsed(freeSearchResult);
	while (markedBlockAsUsed < requiredMemoryBlocks) {
		mMemoryMap[blockToBeMarkedAsUsed.pageId] |= uint32_t(1) << blockToBeMarkedAsUsed.blockId;

		++blockToBeMarkedAsUsed.blockId;
		if (blockToBeMarkedAsUsed.blockId == shiftLimit) {
			blockToBeMarkedAsUsed.blockId = 0;
			++blockToBeMarkedAsUsed.pageId;
		}

		++markedBlockAsUsed;
	}

	void* addr = addrFromBlockIdentifier(freeSearchResult);
	void* baseAddr = addr;
	size_t baseSpace = requiredMemoryBlocks * atomicMemoryBlockSize;
	addr = std::align(bytesAlign, bytes, baseAddr, baseSpace);

	return AllocResult(addr, success);
}

void PoolManager::free(void* memory, size_t bytes, size_t bytesAlign) noexcept {
	size_t baseAlignment = atomicMemoryBlockSize;
	size_t memoryRequiredForAlignmentInBytes = bytesAlign % atomicMemoryBlockSize;

	size_t requiredMemoryBlocks = countRequiredMemoryBlock(bytes) + ((memoryRequiredForAlignmentInBytes > 0) ? countRequiredMemoryBlock(memoryRequiredForAlignmentInBytes) : 0);

	//TODO: fai un po di DBG_ASSERT()

	size_t markedBlockAsUnused = 0;
	blockIdentifier blockToBeMarkedAsUnused(blockIdentifierFromAddr(memory));
	while (markedBlockAsUnused < requiredMemoryBlocks) {
		mMemoryMap[blockToBeMarkedAsUnused.pageId] &= ~(uint32_t(1) << blockToBeMarkedAsUnused.blockId);

		++blockToBeMarkedAsUnused.blockId;
		if (blockToBeMarkedAsUnused.blockId == shiftLimit) {
			blockToBeMarkedAsUnused.blockId = 0;
			++blockToBeMarkedAsUnused.pageId;
		}

		++markedBlockAsUnused;
	}
}

const PoolManager::blockIdentifier PoolManager::freeSearch(uint32_t freeBlocks, const void* const hint) const noexcept {
	size_t resultIt = mPagesCount;
	uint32_t resultBlock = shiftLimit;

	size_t pageOffset = 0;
	uint32_t blockOffsetOnPage = 0; // this can be pre-loaded

	// TODO: ok che inizio a cercare da hint, ma poi non torno piu' indietro!
	if (hint >= mBasePtr) {
		const auto startingOffset = blockIdentifierFromAddr(hint);
		size_t pageOffset = startingOffset.pageId;
		uint32_t blockOffsetOnPage = startingOffset.blockId;
	}

	size_t maxConsecutiveFreeBlocksFound = 0;
	for (pageOffset; (pageOffset < mPagesCount) && (maxConsecutiveFreeBlocksFound < freeBlocks); ++pageOffset) {
		const uint32_t one = 0x00000001;
		for (blockOffsetOnPage; (blockOffsetOnPage < PoolManager::shiftLimit) && (maxConsecutiveFreeBlocksFound < freeBlocks); ++blockOffsetOnPage) {
			if ((mMemoryMap[pageOffset] & (one << blockOffsetOnPage)) == 0) {
				if (maxConsecutiveFreeBlocksFound == 0) {
					resultIt = pageOffset;
					resultBlock = blockOffsetOnPage;
				}
				++maxConsecutiveFreeBlocksFound;
			} else {
				maxConsecutiveFreeBlocksFound = 0;
			}
		}

		blockOffsetOnPage = 0;
	}

	return blockIdentifier(((maxConsecutiveFreeBlocksFound < freeBlocks) ? mPagesCount : resultIt), resultBlock);
}

size_t PoolManager::getMaxAllocationSize() const noexcept {
	size_t maxConsecutiveFreeBlocksFound = 0;

	size_t consecutiveFreeBlocksFound = 0;
	for (size_t it = 0; it < mPagesCount; ++it) {
		const uint32_t one = 0x00000001;
		for (uint32_t i = 0; i < PoolManager::shiftLimit; ++i) {
			if ((mMemoryMap[it] & (one << i)) == 1) {
				maxConsecutiveFreeBlocksFound = std::max<size_t>(maxConsecutiveFreeBlocksFound, consecutiveFreeBlocksFound);
				consecutiveFreeBlocksFound = 0;
			}
			else
				++consecutiveFreeBlocksFound;
		}
	}

	return maxConsecutiveFreeBlocksFound * atomicMemoryBlockSize;
}

size_t PoolManager::getPagesCount() const noexcept {
	return mPagesCount;
}

size_t PoolManager::getNumberOfPagesUsedToManagePages(size_t numberOfPagesToManage) noexcept {
	return (numberOfPagesToManage / numberOfBlocksInPage) + ((numberOfPagesToManage % numberOfBlocksInPage) > 0) ? 1 : 0;
}
