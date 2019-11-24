#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Memory {

		constexpr static uint32_t atomicMemoryBlockSize = 0x800;

		constexpr static uint32_t numberOfBlocksInPage = 32;

		// The number 32 is not to be modified as PoolManager uses an uint32_t to keep track of what blocks are used inside a page
		constexpr static uint32_t atomicMemoryPageSize = numberOfBlocksInPage * atomicMemoryBlockSize;

		constexpr size_t countRequiredMemoryBlock(size_t bytes) {
			size_t minBlockNumber = bytes / atomicMemoryBlockSize;

			return ((bytes % atomicMemoryBlockSize) == 0) ? minBlockNumber : minBlockNumber + 1;
		}

		inline static void* blockAlign(void* base) noexcept {
			uintptr_t aligned = uintptr_t(base);
			return reinterpret_cast<void*>(((aligned / atomicMemoryBlockSize) + (((aligned % atomicMemoryBlockSize) == 0) ? 0 : 1)) * atomicMemoryBlockSize);
		}

	}
}



