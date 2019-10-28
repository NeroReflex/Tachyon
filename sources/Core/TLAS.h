#pragma once

#include "BVHLinearTree.hpp"
#include "BLAS.h"

namespace Tachyon {
	namespace Core {

		/**
		 * The Top-Level Acceleration Structure is used to represent the world as seen from the camera;
		 * in order to achieve the "as seen from the camera" the transformation matrix must be set to the View matrix before rendering a frame.
		 *
		 * Every object in the scene (therefore in a TLAS) is defined with a BLAS that has as the transformation matrix the Model matrix,
		 * so that during a ray-geometry test the geometry to be intersected is transformab using the (M * V) matrix.
		 */
		class TLAS :
			public BVHLinearTree<BLAS, expOfTwoOfMaxBLASElementsInTLAS> {

		public:
			using BVHLinearTree<BLAS, expOfTwoOfMaxBLASElementsInTLAS>::BVHLinearTree;

			constexpr static size_t linearSizeInVec4() noexcept {
				// The first 4 are the view matrix
				return 4 + (NodeData::linearSizeInVec4() * maxNumberOfTreeElements) + (BLAS::linearSizeInVec4() * maxNumberOfElements);
			}

			static void linearize(const TLAS& src, glm::vec4* destination) noexcept {
				// linearize the view matrix
				const auto transformMatrix = src.getTransform();
				destination[0] = transformMatrix[0];
				destination[1] = transformMatrix[1];
				destination[2] = transformMatrix[2];
				destination[3] = transformMatrix[3];

				for (size_t i = 0; i < maxNumberOfTreeElements; ++i) {
					NodeData::linearize(
						src.getNodeIndex(i), 
						&destination[4 + (NodeData::linearSizeInVec4() * i)]);
				}

				for (size_t j = 0; j < maxNumberOfElements; ++j) {
					BLAS::linearize(
						src.getElementAtIndex(j), 
						&destination[4 + (NodeData::linearSizeInVec4() * maxNumberOfTreeElements) + (BLAS::linearSizeInVec4() * j)]);
				}
			}
		};
	}
}