#pragma once

#include "Tachyon.h"

/*
	FOLLOWING DATA STRUCTURES ARE MAINTAINED ALIGNED WITH THE GPU MEMORY
*/

#define expOfTwo_maxModels 10
#define expOfTwo_maxGeometryOnCollection 3
#define expOfTwo_maxCollectionsForModel 15

#define expOfTwo_numberOfLeafsOnTLAS (expOfTwo_maxModels)

#define numberOfTreeElementsToContainExpOfTwoLeafs( expOfTwo ) ((1 << (expOfTwo+1))-1)

namespace Tachyon {
	namespace Rendering {
		/**
		 * This represents a sphere.
		 */
		struct Geometry {
			glm::vec4 center;

			glm::float32 radius;
		};

		struct GeometryCollection {
			Geometry geometry[1 << expOfTwo_maxGeometryOnCollection];
		};

		struct BLASGeometryCollection {
			GeometryCollection collection[1 << expOfTwo_maxCollectionsForModel];
		};

		struct AABB {
			glm::vec4 position;
			glm::vec4 dimensions;
		};

		struct NodeData {
			AABB aabb;
		};

		struct BLAS {
			glm::mat4 ModelMatrix;

			NodeData tree[numberOfTreeElementsToContainExpOfTwoLeafs( expOfTwo_maxCollectionsForModel )];
		};

		struct TLAS {
			NodeData tree[numberOfTreeElementsToContainExpOfTwoLeafs( expOfTwo_maxModels )];
		};
	}
}