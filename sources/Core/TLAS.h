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
			public BVHLinearTree<BLAS, 3> {

		public:
			using BVHLinearTree<BLAS, 3>::BVHLinearTree;
		};
	}
}