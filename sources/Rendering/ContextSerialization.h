#pragma once

#include "Tachyon.h"

// Following structures MUST be kept up-to-date with the raytracer shader

namespace Tachyon {
	namespace Rendering {

		struct Geometry {
			glm::vec4 signature;
			glm::vec4 vertex1_or_sphere_center;
			glm::vec4 vertex2_or_sphere_radius;
			glm::vec4 vertex3_or_nothing;
		};

		struct GeometryCollection {
			Geometry geometry[1 << expOfTwoOfMaxGeometryElementsInCollection]; // geometry slots
		};

		struct AABB {
			glm::vec4 position; // this is the point of the aabb with min x, y and z components
			glm::vec4 dimensions; // this is the amount to be added to aabbVertex to obtain the point with max x, y and z in the aabb
		};

		struct alignas(16) NodeData {
			AABB aabb;
			alignas(sizeof(glm::uint32)) glm::uint32 left; // this can also be the content if right is zero
			alignas(sizeof(glm::uint32))glm::uint32 right; // this is zero if the node is a leaf
		};

		struct BLAS {
			GeometryCollection collection[1 << expOfTwoOfMaxCollectionElementsInBLAS];

			NodeData tree[((1 << expOfTwoOfMaxCollectionElementsInBLAS) * 2) - 1];
		};

		constexpr static glm::uint maxNumberOfTreeElementsInTLAS = 2 * (1 << expOfTwoOfMaxBLASElementsInTLAS) - 1;
		constexpr static glm::uint maxNumberOfBLASInTLAS = 1 << expOfTwoOfMaxBLASElementsInTLAS;
	}
}