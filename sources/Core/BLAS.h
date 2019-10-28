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

			static constexpr size_t getBufferSize() noexcept {
				// To store a linear tree we need the space of the internal transformation matrix (a 4x4 matrix of floats),
				// plus the space to linearize all contained elements, plus the space to linarize all tree nodes
				return sizeof(glm::mat4) + (GeometryCollection::getBufferSize() * BLAS::maxNumberOfElements) + (NodeData::getBufferSize() * BLAS::maxNumberOfTreeElements);
			};

			static void linearizeToBuffer(const BLAS& src, void* dst) noexcept {
				// Linearize GeometryCollection(s)
				for (size_t i = 0; i < BLAS::maxNumberOfElements; ++i) {
					void* baseGeometryCollectionMemoryOffset = reinterpret_cast<void*>(reinterpret_cast<char*>(dst) + (i * GeometryCollection::getBufferSize()));
					GeometryCollection::linearizeToBuffer(src.getElementAtIndex(i), baseGeometryCollectionMemoryOffset);
				}

				for (size_t j = 0; j < BLAS::maxNumberOfTreeElements; ++j) {

				}

				glm::mat4* bufferAsTransformationMatrix = reinterpret_cast<glm::mat4*>(dst);
				bufferAsTransformationMatrix[0] = src.getTransform();

				// TODO: continue to implement linearization...
			}
        };
    }
}