#pragma once

#include "BVHLinearTree.hpp"
#include "BLAS.h"

namespace Tachyon {
	namespace Core {
		class TLAS :
			public BVHLinearTree<BLAS, 3> {

		public:
			using BVHLinearTree<BLAS, 3>::BVHLinearTree;
		};
	}
}