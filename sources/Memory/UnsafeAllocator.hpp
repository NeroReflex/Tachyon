#pragma once

#include "UnsafePoolManager.h"

/*
 * This file implements a thread-unsafe memory UnsafeAllocator using the management strategy provided by UnsafePoolManager,
 * sharing memory amongs copied instances of UnsafeAllocators.
 *
 * Note: this UnsafeAllocator only works with C++20
 */

namespace Tachyon {
	namespace Memory {

		template <class T>
		class UnsafeAllocator {

			template <class T1, class T2>
			friend bool operator==(const UnsafeAllocator<T1>&, const UnsafeAllocator<T2>&) noexcept;

			friend class UnsafeAllocator;

			UnsafePoolManager mManager;

		public:
			typedef ptrdiff_t difference_type;
			typedef size_t size_type;
			typedef T value_type;
			
			UnsafeAllocator(const UnsafePoolManager& mManager) noexcept;

			template <class U>
			UnsafeAllocator(const UnsafeAllocator<U>& src) noexcept
				: mManager(src.mManager) {}

			UnsafeAllocator(const UnsafeAllocator<T>& src) noexcept;

			UnsafeAllocator(UnsafeAllocator<T>&& src) = delete;

			UnsafeAllocator operator=(const UnsafeAllocator<T>&) = delete;

			~UnsafeAllocator();

			T* allocate(size_type n) noexcept;

			void deallocate(T* p, size_type n) noexcept;
		};

		template <class T1, class T2>
		inline bool operator==(const UnsafeAllocator<T1>& lhs, const UnsafeAllocator<T2>& rhs) noexcept {
			return lhs.mManager == rhs.mManager;
		}

		template <class T>
		inline UnsafeAllocator<T>::UnsafeAllocator(const UnsafePoolManager& memoryManager) noexcept
			: mManager(memoryManager) {}

		template <typename T>
		inline UnsafeAllocator<T>::UnsafeAllocator(const UnsafeAllocator<T>& src) noexcept
			:mManager(src.mManager) {}

		template <class T>
		inline UnsafeAllocator<T>::~UnsafeAllocator() {}

		template<class T>
		inline typename T* UnsafeAllocator<T>::allocate(typename UnsafeAllocator<T>::size_type n) noexcept {
			auto allocResult = mManager.malloc(n, alignof(T));

			DBG_ASSERT(allocResult.success);

			return reinterpret_cast<T*>(allocResult.result);
		}

		template<class T>
		inline void UnsafeAllocator<T>::deallocate(typename T* p, typename UnsafeAllocator<T>::size_type n) noexcept {
			mManager.free(p, n, alignof(T));
		}

	}
}