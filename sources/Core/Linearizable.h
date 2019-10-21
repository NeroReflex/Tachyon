#pragma once

#include "Tachyon.h"

namespace Tachyon {
	namespace Core {
		class Linearizable {
		public:
			/**
			 * Get the size of a buffer that can contains the linearized object.
			 *
			 * @return the resulting size in bytes
			 */
			virtual size_t getLinearBufferSize() const noexcept = 0;

			/**
			 * Linearize the current object to the provided buffer. Whenever possible std430 shuold be followed.
			 *
			 * @param buffer the buffer to be written
			 */
			virtual void linearizeToBuffer(void* buffer) const noexcept = 0;
		};
	}
}