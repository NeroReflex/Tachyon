#pragma once

#include "MemoryPool.h"

namespace Tachyon {
	namespace Memory {

		class PoolManager {
			constexpr static uint32_t shiftLimit = 32;

			struct AllocResult {
				void* result;

				const bool success;

				inline AllocResult(void* const result, const bool& success) noexcept : result(result), success(success) {};
			};

		public:
			static size_t getNumberOfPagesUsedToManagePages(size_t numberOfPagesToManage) noexcept;

			/**
			 * Construct a pool memory manager for pagesCount atomic memory pages.
			 *
			 * The allocation-tracking structure will be stored in the given fixeded memory buffer.
			 */
			PoolManager(size_t pagesCount, void* const basePtr, uint32_t* const fixedMem = nullptr, bool reset = true) noexcept;

			PoolManager() = delete;

			PoolManager(const PoolManager&) = delete;

			PoolManager(PoolManager&&) = delete;

			~PoolManager();

			AllocResult malloc(size_t bytes, size_t bytesAlign = 1, const void* const hint = NULL) noexcept;

			void free(void* memory, size_t bytes, size_t bytesAlign = 1) noexcept;

			size_t getMaxAllocationSize() const noexcept;

			size_t getPagesCount() const noexcept;
			
		private:
			struct blockIdentifier {
				size_t pageId;

				size_t blockId;

				inline blockIdentifier(const size_t& id, size_t block) noexcept : pageId(id), blockId(block) {}
			};

			void* addrFromBlockIdentifier(const blockIdentifier&) const noexcept;

			blockIdentifier blockIdentifierFromAddr(const void* const addr) const noexcept;

			const blockIdentifier freeSearch(uint32_t freeBlocks, const void* const hint = NULL) const noexcept;

		private:
			void* const mBasePtr;

			uint32_t* const mMemoryMap;

			size_t mPagesCount;
		};
	}
}
