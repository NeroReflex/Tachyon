#pragma once

#include "PoolManager.h"

/*
This file implements a thread-unsafe memory allocator using the management strategy provided by PoolManager, sharing memory amongs copied instances of allocator.
*/

namespace Tachyon {
	namespace Memory {

		template <class T>
		class Allocator {

			friend class Allocator;

		public:
			typedef size_t size_type;
			typedef T value_type;
			typedef value_type* pointer;
			typedef value_type& reference;
			typedef const value_type* const_pointer;
			typedef const value_type& const_reference;
			typedef ptrdiff_t difference_type;
			
			template <class U>
			struct rebind {
				typedef Allocator<U> other;
			};

			Allocator(size_t pagesCount) noexcept;

			template <class U>
			Allocator(const Allocator<U>& src) noexcept
				: mMemPool(src.mMemPool), mManager(src.mManager.getPagesCount(), blockAlign(mMemPool), reinterpret_cast<uint32_t*>(blockAlign(mMemPool)), false) {}

			Allocator(const Allocator<T>& src) noexcept;

			Allocator(Allocator<T>&& src) = delete;

			Allocator operator=(const Allocator<T>&) = delete;

			~Allocator();

			pointer address(reference x) const noexcept;

			pointer allocate(size_type n, const void* hint = NULL) noexcept;

			void deallocate(pointer p, size_type n) noexcept;

			size_type max_size() const noexcept;

			template <class U>
			void destroy(U* p) noexcept {
				p->~U();
			}

			template <class U, class... Args>
			void construct(U* p, Args&&... args) noexcept {
				::new ((void*)p) U(std::forward<Args>(args)...);
			}

		private:
			bool mMemoryPoolOwner = false;

			void* mMemPool;

			PoolManager mManager;
		};

		template<>
		class Allocator<void> {

			friend class Allocator;

		public:
			typedef size_t size_type;
			typedef void* pointer;
			typedef const void* const_pointer;

			template <class U>
			struct rebind {
				typedef Allocator<U> other;
			};

			Allocator(size_t pagesCount) noexcept;

			template <class U>
			Allocator(const Allocator<U>& src) noexcept
				: mMemPool(src.mMemPool), mManager(src.mManager.getPagesCount(), blockAlign(mMemPool), reinterpret_cast<uint32_t*>(blockAlign(mMemPool)), false) {}

			Allocator(const Allocator<void>& src) noexcept;

			Allocator(Allocator<void>&&) = delete;

			Allocator operator=(const Allocator<void>&) = delete;

			~Allocator();

			pointer allocate(size_type n, const void* hint = NULL) noexcept;

			void deallocate(pointer p, size_type n) noexcept;

			size_type max_size() const noexcept;

			template <class U>
			void destroy(U* p) noexcept {
				p->~U();
			}

			template <class U, class... Args>
			void construct(U* p, Args&&... args) noexcept {
				::new ((void*)p) U(std::forward<Args>(args)...);
			}

		private:
			bool mMemoryPoolOwner = false;

			void* mMemPool;

			PoolManager mManager;
		};

		template <class T>
		inline Allocator<T>::Allocator(size_t pagesCount) noexcept
			: mMemoryPoolOwner(true), mMemPool(calloc((pagesCount * numberOfBlocksInPage) + 1, atomicMemoryBlockSize)), mManager(pagesCount, blockAlign(mMemPool)) {}

		template <typename T>
		inline Allocator<T>::Allocator(const Allocator<T>& src) noexcept
			: mMemPool(src.mMemPool), mManager(src.mManager.getPagesCount(), blockAlign(mMemPool), reinterpret_cast<uint32_t*>(blockAlign(mMemPool)), false) {}

		template <class T>
		inline Allocator<T>::~Allocator() {
			if (mMemoryPoolOwner) free(mMemPool);
		}

		template<class T>
		inline typename Allocator<T>::pointer Allocator<T>::address(typename Allocator<T>::reference x) const noexcept {
			return &x;
		}

		template<class T>
		inline typename Allocator<T>::pointer Allocator<T>::allocate(typename Allocator<T>::size_type n, const void* hint) noexcept {
			auto allocResult = mManager.malloc(n, alignof(T), hint);

			DBG_ASSERT(allocResult.success);

			return reinterpret_cast<Allocator<T>::pointer>(allocResult.result);
		}

		template<class T>
		inline void Allocator<T>::deallocate(typename Allocator<T>::pointer p, typename Allocator<T>::size_type n) noexcept {
			mManager.free(p, n, alignof(T));
		}

		template<class T>
		inline typename Allocator<T>::size_type Allocator<T>::max_size() const noexcept {
			return mManager.getMaxAllocationSize();
		}
	}
}