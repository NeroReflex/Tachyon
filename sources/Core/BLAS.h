#pragma once

#include "BVHLinearTree.hpp"
#include "GeometryCollection.h"

namespace Tachyon {
    namespace Core {
        class BLAS :
            public BVHLinearTree<GeometryCollection, 3> {

        public:
            using BVHLinearTree<GeometryCollection, 3>::BVHLinearTree;

			AABB bvBase() const noexcept;
        };
    }
}