#pragma once

#include "MemoryPool.h"

namespace Tachyon {
	namespace Memory {

		class UnsafePoolManager {
			typedef uint64_t index_t;
			typedef uint8_t managementType;
			
			struct AllocResult {
				void* result;

				const bool success;

				inline AllocResult(const bool& success, void* const result) noexcept : result(result), success(success) {};
			};

		public:
			UnsafePoolManager() = delete;
			
			/**
			 * Construct a pool memory manager for pagesCount atomic memory pages.
			 * 
			 * @param pagesCount the number of pages to be managed
			 * @param basePtr the base pointer to be managed. MUST be aligned to atomicMemoryBlockSize
			 */
			UnsafePoolManager(managementType blockCount, void* const basePtr, managementType* managementStructure ) noexcept;

			UnsafePoolManager(const UnsafePoolManager&) noexcept;

			UnsafePoolManager(UnsafePoolManager&&) noexcept;
			
			UnsafePoolManager& operator=(const UnsafePoolManager&) noexcept;

			~UnsafePoolManager();

			AllocResult malloc(size_t bytes, size_t bytesAlign = 1) noexcept;

			void free(void* memory, size_t bytesSize, size_t bytesAlign = 1) noexcept;

		private:
			inline size_t queryRequiredBlockCount(index_t bytesSize, index_t bytesAlign) {
				return ((bytesAlign != 1) ? 1 : 0) + (bytesSize / atomicMemoryBlockSize) + (((bytesSize % atomicMemoryBlockSize) == 0) ? 0 : 1);
				//                                                                     ^
				//                                                      alignment round-up	
			}
			
			inline void* addressFromIndex(index_t idx) const noexcept {
				return (void*)(index_t(uintptr_t(mBasePtr)) + (atomicMemoryBlockSize * idx));
			};
			
			inline index_t indexFromAddress(void* ptr) const noexcept {
				DBG_ASSERT( (uintptr_t(ptr) >= uintptr_t(mBasePtr)) );
				return index_t(uintptr_t(ptr) - uintptr_t(mBasePtr)) / index_t(atomicMemoryBlockSize);
			}
			
			void* mBasePtr;

			managementType* mMemoryMap;

			managementType mBlockCount;
		};
	}
}

