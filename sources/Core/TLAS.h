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

			static constexpr size_t getBufferSize() noexcept {
				// To store a linear tree we need the space of the internal transformation matrix (a 4x4 matrix of floats),
				// plus the space to linearize all contained elements, plus the space to linarize all tree nodes
				return sizeof(glm::mat4) +
					(GeometryCollection::getBufferSize() * TLAS::maxNumberOfElements) +
					(NodeData::getBufferSize() * TLAS::maxNumberOfTreeElements);
			};

			static void linearizeToBuffer(const TLAS& src, void* dst) noexcept {
				glm::mat4* bufferAsTransformationMatrix = reinterpret_cast<glm::mat4*>(dst);
				bufferAsTransformationMatrix[0] = src.getTransform();


				// TODO: continue to implement linearization...
			}
		};
	}
}