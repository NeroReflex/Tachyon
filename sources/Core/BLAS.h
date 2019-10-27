#pragma once

#include "BVHLinearTree.hpp"
#include "GeometryCollection.h"

namespace Tachyon {
    namespace Core {

		/**
		 * The Bottom-Level Acceleration Structure is used to represent a mesh or a set of object bounds to each others.
		 * The internal transformation matrix must be set to the object Model matrix (rigid transformation matrix) before rendering a frame,
		 * this way the ray-geometry intersaction will be performed with the geometry transformed by the Model matrix as calculated by a physics engin for example.
		 */
        class BLAS :
            public BVHLinearTree<GeometryCollection, expOfTwoOfMaxCollectionElementsInBLAS> {

        public:
            using BVHLinearTree<GeometryCollection, expOfTwoOfMaxCollectionElementsInBLAS>::BVHLinearTree;

			AABB bvBase() const noexcept;
        };
    }
}