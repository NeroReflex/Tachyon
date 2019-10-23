#pragma once

#include "Geometry.h"

namespace Tachyon {
	namespace Core {

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		class Collection {

		protected:
			constexpr static size_t maxNumber = 2^expOfTwoOfMaxNumberOfElements;

		public:
			Collection() noexcept;

			Collection(const T* data, size_t count) noexcept;

			Collection(const std::initializer_list<T>& elements) noexcept;

			Collection(const Collection<T, expOfTwoOfMaxNumberOfElements>& src) noexcept;

			Collection& operator=(const Collection<T, expOfTwoOfMaxNumberOfElements>& src) noexcept;

			void push(const T& element) noexcept;

			size_t getSize() const noexcept;

			void foreach(const std::function<void(T&)>& fn) noexcept;

			void foreach(const std::function<void(const T&)>& fn) const noexcept;

		private:
			std::array<T, maxNumber> mElements;

			size_t mElementsCount;
		};

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		Collection<T, expOfTwoOfMaxNumberOfElements>::Collection() noexcept : mElementsCount(0) {}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		Collection<T, expOfTwoOfMaxNumberOfElements>::Collection(const T* data, size_t count) noexcept : Collection() {
			for (size_t i = 0; i < count; ++i)
				push(data[i]);
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		Collection<T, expOfTwoOfMaxNumberOfElements>::Collection(const std::initializer_list<T>& elements) noexcept : Collection() {
			for (const auto& element : elements)
				push(element);
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		Collection<T, expOfTwoOfMaxNumberOfElements>::Collection(const Collection<T, expOfTwoOfMaxNumberOfElements>& src) noexcept : Collection(src.mElements.data(), src.mElementsCount) {}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		Collection& Collection<T, expOfTwoOfMaxNumberOfElements>::operator=(const Collection<T, expOfTwoOfMaxNumberOfElements>& src) noexcept {
			mElementsCount = 0;

			for (size_t i = 0; i < src.mElementsCount; ++i)
				push(src.mElements[i]);

			return *this;
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		void Collection<T, expOfTwoOfMaxNumberOfElements>::push(const T& element) noexcept {
			DBG_ASSERT(mElementsCount < Collection<T, expOfTwoOfMaxNumberOfElements>::maxNumber);

			// Store the geometry and increase the counter of stored geometry
			mElements[mElementsCount++] = element;
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		size_t Collection<T, expOfTwoOfMaxNumberOfElements>::getSize() const noexcept {
			return mElementsCount;
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		void Collection<T, expOfTwoOfMaxNumberOfElements>::foreach(const std::function<void(T&)>& fn) noexcept {
			for (size_t i = 0; i < mElementsCount; ++i)
				fn(mElements[i]);
		}

		template <class T, size_t expOfTwoOfMaxNumberOfElements>
		void Collection<T, expOfTwoOfMaxNumberOfElements>::foreach(const std::function<void(const T&)>& fn) const noexcept {
			for (size_t i = 0; i < mElementsCount; ++i)
				fn(mElements[i]);
		}
	}
}