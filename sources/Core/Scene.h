#pragma once

#include "BVHLinearTree.hpp"
#include "BLAS.h"

namespace Tachyon {
	namespace Core {

		class Scene {
		public:
			// GeometryCollection
			constexpr static glm::uint32 maxNumberOfGeometryInCollection = 1 << expOfTwoOfMaxGeometryElementsInCollection;
			
			// BLAS
			constexpr static glm::uint32 maxNumberOfGeometryCollectionsInBLAS = 1 << expOfTwoOfMaxCollectionElementsInBLAS;

			// TLAS
			constexpr static glm::uint maxNumberOfTreeElementsInTLAS = 2 * (1 << expOfTwoOfMaxBLASElementsInTLAS) - 1;
			constexpr static glm::uint maxNumberOfBLASInTLAS = 1 << expOfTwoOfMaxBLASElementsInTLAS;
			constexpr static glm::uint maxTLASTreeDepth = 32; // As the TLAS-tree traverse algorithm is iterative and of contant size there is a maximum supported depth.

			/**
			 * This is the geometry as it is represented on the GPU
			 * or whatever device is used to perform the raytracing algorithm.
			 */
			struct alignas(sizeof(glm::vec4)) Geometry {
				glm::vec4 signature;
				glm::vec4 vertex1_or_sphere_center;
				glm::vec4 vertex2_or_sphere_radius;
				glm::vec4 vertex3_or_nothing;

				inline Geometry() noexcept : signature(0, 0, 0, 0), vertex1_or_sphere_center(0, 0, 0, 1), vertex2_or_sphere_radius(0, 0, 0, 0), vertex3_or_nothing(0, 0, 0, 0) {}
			};

			static const Geometry emptyGeometry;

			struct alignas(sizeof(glm::uint)) GeometryCollection {
				/**
				 * This is the collection of indices of referenced geometry.
				 *
				 * The number of referenced geometry is the number of elements not equal to an empty geometry.
				 */
				glm::uint geometryIndex[maxNumberOfGeometryInCollection];

				GeometryCollection() noexcept : geometryIndex() { for (glm::uint32 i = 0; i < maxNumberOfGeometryInCollection; ++i) geometryIndex[i] = 0; }
			};

			static const GeometryCollection emptyGeometryCollection;

			/**
			 * This represents an Axis-Aligned Bounding Box.
			 *
			 *    +--------------+ < Max vertex (max x, y and z amongs all vertices)
			 *   /|             /|
			 *  / |            / |
			 * *--+-----------*  |
			 * |  |           |  | Depth
			 * |  |           |  |
			 * |  |           |  |
			 * |  +-----------+--+
			 * | /            | /
			 * |/             |/ Width
			 * *--------------*
			 * ^    Length
			 * Position vertex (minimum x, y and z amongs all vertices)
			 */
			struct AABB {

				/**
				 * This is the position vertex of the AABB.
				 *
				 * The used format is vec4(x, y, z, 1)
				 */
				glm::vec4 position;

				/**
				 * This is the collection of AABB dimensions.
				 *
				 * The used format is vec4(Length, Depth, Width, 0)
				 * Note: fastest way to find the max vertex of the AABB: position+dimensions
				 * Note: an AABB with one component of dimensions (except w) equals to zero is an empty AABB and cannot be hit by anything.
				 */
				glm::vec4 dimensions;

				inline AABB(glm::vec4 pos = glm::vec4(0, 0, 0, 1), glm::vec4 dim = glm::vec4(0, 0, 0, 0)) noexcept : position(std::move(pos)), dimensions(std::move(dim)) {}

				/**
				 * Apply a transform to the given AABB.
				 *
				 * @param starting the AABB transformation will be applied to
				 * @transform the transform to be applied to all vertices of the AABB, given as a 4x4 matrix
				 * @return thetransformed AABB
				 */
				static AABB applyTransform(const AABB& starting, glm::mat4 transform) noexcept;
			};

			/**
			 * This represents a node of a generic Acceleration Structure used for raytracing.
			 */
			struct alignas(16) NodeData {
				/**
				 * The node AABB that can be either:
				 *     - The content AABB if the current node is a leaf
				 *     - The union of left and right AABBs
				 */
				AABB aabb;

				/**
				 * This is the index of the left NodeData on the array used to store the binary tree,
				 * but can also be the content if right is zero, and therefore the NodeData is a leaf.
				 *
				 * The content can be intended as:
				 *    - on a TLAS: the content is the index of the referenced BLAS
				 *    - on a BLAS: the content is the index of the referecend GeometryCollection
				 */
				alignas(sizeof(glm::uint32)) glm::uint32 left;

				/**
				 * This is the index of the right NodeData on the array used to store the binary tree,
				 * but if this is zero than the NodeData is a leaf and therefore left is the index of the leaf content.
				 */
				alignas(sizeof(glm::uint32)) glm::uint32 right;

				inline NodeData(AABB bv = AABB(), glm::uint32 l = 0, glm::uint32 r = 0) noexcept : aabb(std::move(bv)), left(std::move(l)), right(std::move(r)) {}
			};

			template <glm::uint N>
			struct AccelerationStructure {
				NodeData tree[(1 << (N + 1)) - 1];

				inline AccelerationStructure() noexcept {}

				inline bool isRoot(glm::uint index) const noexcept { return index == 0; }
			};

			typedef AccelerationStructure<expOfTwoOfMaxCollectionElementsInBLAS> BLAS;

			typedef AccelerationStructure<expOfTwoOfMaxBLASElementsInTLAS> TLAS;

		public:
			Scene() noexcept;

		private:
			/**
			 * This is the tree structure of the TLAS
			 */
			TLAS mTLAS;

			/**
			 * This is the collection of BLAS.
			 * 
			 * Each BLAS has its model matrix stored in mBLAS_ModelMatrix at the same index.
			 */
			std::array<BLAS, maxNumberOfBLASInTLAS> mBLAS;

			/**
			 * This is the collection of Model Matrices for objects to be rendered.
			 *
			 * The model matrix of the BLAS representing the object at index i is also at index i.
			 */
			std::array<glm::mat4, maxNumberOfBLASInTLAS> mBLAS_ModelMatrix;

			/**
			 * This is all the geometry data referenced by geometry collections.
			 * 
			 * The geometry data MUST always contain as the first element a meaningless geometry
			 * that gets always discarded by the renderer and leads to an empty/invalid AABB with area and volume equals to zero.
			 */
			std::vector<Geometry> mGeometryData;

			/**
			 * This is all the geometry collection data referenced by every BLAS in the scene.
			 *
			 * The geometry collection data MUST always contain as the first element a meaningless geometry
			 * that gets always discarded by the renderer and leads to an empty/invalid AABB with area and volume equals to zero.
			 */
			std::vector<GeometryCollection> mGeometryCollection;
		};

	}
}