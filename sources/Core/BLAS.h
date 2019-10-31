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

			void linearize(Tachyon::Rendering::BLAS& blas) const noexcept;

			constexpr static size_t linearSizeInVec4() noexcept {
				// The first 4 are the model matrix
				return 4 + (NodeData::linearSizeInVec4() * maxNumberOfTreeElements) + (GeometryCollection::linearSizeInVec4() * maxNumberOfElements);
			}

			static void linearizeToVec4(const BLAS& src, glm::vec4* destination) noexcept {
				// linearize the view matrix
				const auto transformMatrix = src.getTransform();
				destination[0] = transformMatrix[0];
				destination[1] = transformMatrix[1];
				destination[2] = transformMatrix[2];
				destination[3] = transformMatrix[3];

				for (size_t i = 0; i < maxNumberOfTreeElements; ++i) {
					NodeData::linearizeToVec4(
						src.getNodeIndex(i), 
						&destination[4 + (NodeData::linearSizeInVec4() * i)]);
				}

				for (size_t j = 0; j < maxNumberOfElements; ++j) {
					GeometryCollection::linearizeToVec4(
						src.getElementAtIndex(j), 
						&destination[4 + (NodeData::linearSizeInVec4() * maxNumberOfTreeElements) + (GeometryCollection::linearSizeInVec4() * j)]);
				}
			}
        };
    }
}