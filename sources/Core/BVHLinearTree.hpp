#pragma once

#include "Collection.hpp"
#include "AABB.h"

namespace Tachyon {
	namespace Core {
		/**
		 * This class represents a linearized BVH-tree and its management structure.
		 *
		 * A linearized BVH-tree is used to reach the maximum performance on rendering stage!
		 */
		template <class ContentType, size_t N>
		class BVHLinearTree :
			virtual public RayInterceptable {

			constexpr static glm::uint16 twoExpOfMaxElementsNumber = N;
			constexpr static size_t maxNumberOfElements = 1 << twoExpOfMaxElementsNumber;
			constexpr static size_t maxNumberOfTreeElements = (maxNumberOfElements * 2) - 1;

		public:
			BVHLinearTree(const BVHLinearTree<ContentType, N>&) noexcept;
			
			BVHLinearTree& operator=(const BVHLinearTree<ContentType, N>&) noexcept;

			virtual ~BVHLinearTree();

			BVHLinearTree() noexcept;

			/**
			 * Insert an element into the BVH tree.
			 */
			void insert(const ContentType& uniqueInstance) noexcept;

			/**
			 * Traverse each node of the tree to execute the given function on each leaf.
			 */
			void traverse(const std::function<void(const ContentType&)>& fn, UnsignedType root = 0) const noexcept;

			/**
			 * Check if the given ray collides with any of the stored elements transformed by the internal matrix and the give one.
			 *
			 * @return true if the given ray intersects one or more stored objects
			 */
			bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept final;

			/**
			 * Check if the given ray collides with any of the stored elements transformed by the internal matrix and the give one,
			 * and if that's the case the ray-geometry intersection gets updated with the closes hit.
			 *
			 * @return true if the given ray intersects one or more stored objects
			 */
			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept final;

			void setTransform(const glm::mat4& transformationMatrix = glm::mat4(1)) noexcept;

			const glm::mat4& getTransform() const noexcept;

		private:
			glm::mat4 mTransformationMatrix;

			/**
			 * Keeps track of free nodes
			 */
			std::list<glm::uint64> mFreeNodes;

			bool isBVHitByRay(const Ray& ray, glm::mat4 transform = glm::mat4(1), UnsignedType root = 0) const noexcept;

			bool intersects(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1), UnsignedType root = 0) const noexcept;

		protected:
			bool isRoot(UnsignedType index) const noexcept;

			bool isLeaf(UnsignedType index) const noexcept;

			AABB& bv(UnsignedType index) noexcept;

			const AABB& bv(UnsignedType index) const noexcept;

			bool isFree(UnsignedType index) const noexcept;

			void refreshBVH(UnsignedType index) noexcept;

#pragma pack(push, 16)
			struct NodeData {
				AABB bvh;

				union {
					struct {
						UnsignedType mLeft;
						UnsignedType mRight;

						void reset(UnsignedType left, UnsignedType right) noexcept {
							mLeft = left;
							mRight = right;
						};
					} tree;
					UnsignedType content;
				};
#pragma pack(pop)

				NodeData() noexcept;

				bool isLeaf() const noexcept;

				void becomeTree(UnsignedType left, UnsignedType right) noexcept;

				void becomeLeaf(UnsignedType contentLocation) noexcept;

				static NodeData createTree(UnsignedType left, UnsignedType right) noexcept;

				static NodeData createLeaf(UnsignedType contentLocation) noexcept;

				static constexpr size_t getBufferSize() noexcept {
					// To store a node data we need two vec4 and two uint32 elements:
					// One vec4 is the minimum vertex, the other is vec4(length, depth, width, 0)
					// The first uint32 is the left node index / content, the other is the right node index, or zero if the node is a leaf

					// TODO: change the (4) with the amound of bytes needed to store a node
					return 2 * sizeof(glm::vec4) + 2 * sizeof(glm::uint32);
				};

				static void linearizeToBuffer(const NodeData& src, void* dst) noexcept {
					// Make sure the linearization won't take more space than what it is necessary
					static_assert((sizeof(glm::vec4) / 4) == sizeof(glm::uint32));

					glm::vec4* bufferAsVector = reinterpret_cast<glm::vec4*>(dst);
					bufferAsVector[0] = glm::vec4(src.bvh.getPosition(), 1.0);
					bufferAsVector[1] = glm::vec4(src.bvh.getLength(), src.bvh.getDepth(), src.bvh.getWidth(), 0);

					glm::uint32* bufferAsUint = reinterpret_cast<glm::uint32*>(&bufferAsVector[2]);
					bufferAsUint[0] = src.tree.mLeft;
					bufferAsUint[1] = src.tree.mRight;
				}
			};

			UnsignedType insert(NodeData node, UnsignedType root = 0) noexcept;

			Collection<ContentType, N> mContentCollection;
			std::array<NodeData, maxNumberOfTreeElements> mLinearTree;


		public:
			static constexpr size_t getBufferSize() noexcept {
				// To store a linear tree we need the space of the internal transformation matrix (a 4x4 matrix of floats),
				// plus the space to linearize all contained elements, plus the space to linarize all tree nodes
				return sizeof(glm::mat4) + (ContentType::getBufferSize() * maxNumberOfElements) + (NodeData::getBufferSize() * maxNumberOfTreeElements);
			};

			static void linearizeToBuffer(const BVHLinearTree<ContentType, N>& src, void* dst) noexcept {
				glm::mat4* bufferAsTransformationMatrix = reinterpret_cast<glm::mat4*>(dst);
				bufferAsTransformationMatrix[0] = src.mTransformationMatrix;


				// TODO: continue to implement linearization...
			}
		};

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::BVHLinearTree() noexcept 
			: mTransformationMatrix(glm::mat4(1)) {
			for (size_t i = 0; i < BVHLinearTree::maxNumberOfTreeElements; ++i)
				mFreeNodes.emplace_back(i);
		}

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::BVHLinearTree(const BVHLinearTree<ContentType, N>& src) noexcept
			: mTransformationMatrix(src.mTransformationMatrix),
			mFreeNodes(src.mFreeNodes),
			mContentCollection(src.mContentCollection),
			mLinearTree(src.mLinearTree) {}

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>& BVHLinearTree<ContentType, N>::operator=(const BVHLinearTree<ContentType, N>& src) noexcept {
			if (&src != this) {
				mTransformationMatrix = src.mTransformationMatrix;
				mFreeNodes = src.mFreeNodes;
				mContentCollection = src.mContentCollection;
				mLinearTree = src.mLinearTree;
			}

			return *this;
		}

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::~BVHLinearTree() {}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::setTransform(const glm::mat4& transformationMatrix) noexcept {
			mTransformationMatrix = transformationMatrix;
		}

		template <class ContentType, size_t N>
		const glm::mat4& BVHLinearTree<ContentType, N>::getTransform() const noexcept {
			return mTransformationMatrix;
		}

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::NodeData::NodeData() noexcept {
			tree.reset(0, 0);
		}

		template <class ContentType, size_t N>
		typename BVHLinearTree<ContentType, N>::NodeData BVHLinearTree<ContentType, N>::NodeData::createLeaf(UnsignedType contentLocation) noexcept {
			NodeData result;
			result.becomeLeaf(contentLocation);

			return result;
		}

		template <class ContentType, size_t N>
		typename BVHLinearTree<ContentType, N>::NodeData BVHLinearTree<ContentType, N>::NodeData::createTree(UnsignedType left, UnsignedType right) noexcept {
			NodeData result;
			result.becomeTree(left, right);

			return result;
		}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::NodeData::becomeLeaf(UnsignedType contentLocation) noexcept {
			// A BVH node is a tree iif the right tree is zero
			tree.reset(0, 0);

			this->content = contentLocation;
		}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::NodeData::becomeTree(UnsignedType left, UnsignedType right) noexcept {
			// the right "pointer" will be occupied, so that this node will never get confused with a leaf
			tree.reset(left, right);
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::NodeData::isLeaf() const noexcept {
			// mLeft DOES occupy the same memory region of mLeft, so checking if an object is a leaf only involves mRight
			return !tree.mRight;
		}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::insert(const ContentType& element) noexcept {
			// No free space remaining on the BVH or nothing to be addedd
			DBG_ASSERT((!mContentCollection.isFull()));

			// occupy the first free location (its index is getSize())
			const auto occupiedContentLocation = mContentCollection.getSize();
			mContentCollection.push(element); // This will also update the content count

			// create the leaf to be added
			auto leaf = NodeData::createLeaf(occupiedContentLocation);
			leaf.bvh = element.bvBase();

			// add the leaf node to the tree
			insert(std::move(leaf));
		}

		template <class ContentType, size_t N>
		UnsignedType BVHLinearTree<ContentType, N>::insert(NodeData node, UnsignedType root) noexcept {
			if (isFree(root)) { // if this is a free tree create a new one
				mLinearTree[root] = node;

				// remove root from the list of free nodes as we have occupied it right now
				mFreeNodes.remove(root);
			}
			else {
				if (isLeaf(root)) { // convert the leaf on root position to a tree
					auto freeNodeCIt = mFreeNodes.cbegin();
					const auto newNode = *freeNodeCIt;

					// Take the free node and use it to create the node that will hold two leafs
					NodeData& newTree = mLinearTree[newNode];

					// Erase the node that has been taken and prepare to fetch another free node
					freeNodeCIt = mFreeNodes.erase(freeNodeCIt);

					// thanks to the base of recursion this node will be written to a free block...
					UnsignedType newLeft = newNode, newRight = insert(std::move(node), *freeNodeCIt);

					// Make sure to get the correct order of positioning
					const auto boundingBothBVH = AABB(bv(root), node.bvh);
					if (AABB::SAH(boundingBothBVH, bv(root)) > AABB::SAH(boundingBothBVH, node.bvh)) {
						// Current leaf will become the RIGHT node of the parent tree to be created,
						// New leaf will become the LEFT node of the parent tree to be created
						std::swap(newLeft, newRight);
					}

					// The newly created tree will contains the current node, which is a leaf and the given node (that can either be a leaf or a tree)
					newTree = NodeData::createTree(newLeft, newRight);

					// Swap the new tree with the old one, so that the root of the tree does remains the root and not a leaf
					std::swap(newTree, mLinearTree[root]);
				}
				else {
					// insert at the left or right (depending on the SAH: lower->left, higher->right)
					const auto boundingBothBVH = AABB(node.bvh, bv(root));
					const auto boundingLeftAndNewBVH = AABB(node.bvh, bv(mLinearTree[root].tree.mLeft));
					const auto boundingRightAndNewBVH = AABB(node.bvh, bv(mLinearTree[root].tree.mRight));
					if (AABB::SAH(boundingBothBVH, boundingLeftAndNewBVH) < AABB::SAH(boundingBothBVH, boundingRightAndNewBVH))
						mLinearTree[root].tree.mLeft = insert(std::move(node), mLinearTree[root].tree.mLeft);
					else
						mLinearTree[root].tree.mRight = insert(std::move(node), mLinearTree[root].tree.mRight);
				}
			}

			// create the bounding box
			refreshBVH(root);

			return root;
		}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::refreshBVH(UnsignedType index) noexcept {
			if (isLeaf(index)) {
				// sync this BV shape to the base rigid object BV multiplied by the rigid body current Model latrix ...
				const auto& element = mContentCollection[mLinearTree[index].content];

				// TODO: update the AABB using the tranformation matrix OF THE current BVH tree
				bv(index) = element.bvBase(); // AABB(element.bvBase(), getTransform());
			}
			else {
				bv(index) = AABB(bv(mLinearTree[index].tree.mLeft), bv(mLinearTree[index].tree.mRight));
			}
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::isFree(UnsignedType index) const noexcept {
			const auto findResult = std::find(mFreeNodes.cbegin(), mFreeNodes.cend(), index);
			const bool free = findResult != mFreeNodes.cend();
			return free;
		}

		template <class ContentType, size_t N>
		void BVHLinearTree<ContentType, N>::traverse(const std::function<void(const ContentType&)>& fn, UnsignedType root) const noexcept {
			if (isFree(root)) return;

			if (isLeaf(root)) {
				fn(mContentCollection[mLinearTree[root].content]);
			}
			else {
				traverse(fn, mLinearTree[root].tree.mLeft);
				traverse(fn, mLinearTree[root].tree.mRight);
			}
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::isBVHitByRay(const Ray& ray, glm::mat4 transform, UnsignedType root) const noexcept {
			if (isFree(root)) return false;

			if (isLeaf(root)) {
				return mContentCollection[mLinearTree[root].content].isHitBy(ray, transform);
			} else if (bv(root).isHitBy(ray, transform)) {
				bool leftHit = false, rightHit = false;

				if (bv(mLinearTree[root].tree.mLeft).isHitBy(ray, transform)) {
					leftHit = isBVHitByRay(ray, transform, mLinearTree[root].tree.mLeft);
				}

				if (bv(mLinearTree[root].tree.mRight).isHitBy(ray, transform)) {
					rightHit = isBVHitByRay(ray, transform, mLinearTree[root].tree.mRight);
				}

				return leftHit || rightHit;
			}

			return false;
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::intersects(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform, UnsignedType root) const noexcept {
			if (isFree(root)) return false;

			bool match = false;

			if (isLeaf(root)) {
				return mContentCollection[mLinearTree[root].content].intersection(ray, minDistance, maxDistance, isecInfo, transform);
			} else if (bv(root).isHitBy(ray, transform)) {
				bool leftHit = false, rightHit = false;

				if (bv(mLinearTree[root].tree.mLeft).isHitBy(ray, transform)) {
					leftHit = intersects(ray, minDistance, maxDistance, isecInfo, transform, mLinearTree[root].tree.mLeft);
				}

				if (bv(mLinearTree[root].tree.mRight).isHitBy(ray, transform)) {
					RayGeometryIntersection temp;
					rightHit = intersects(ray, minDistance, maxDistance, temp, transform, mLinearTree[root].tree.mRight);

					if (!leftHit || (temp.getDistance() < isecInfo.getDistance())) {
						isecInfo = temp;
					}
				}

				return leftHit || rightHit;
			}

			return false;
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::isLeaf(UnsignedType index) const noexcept {
			const auto& selectedNode = mLinearTree[index];

			bool result = selectedNode.isLeaf();

			return result;
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::isRoot(UnsignedType index) const noexcept {
			return index == 0;
		}

		template <class ContentType, size_t N>
		AABB& BVHLinearTree<ContentType, N>::bv(UnsignedType index) noexcept {
			return mLinearTree[index].bvh;
		}

		template <class ContentType, size_t N>
		const AABB& BVHLinearTree<ContentType, N>::bv(UnsignedType index) const noexcept {
			return mLinearTree[index].bvh;
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::isHitBy(const Ray& ray, glm::mat4 transform) const noexcept {
			return isBVHitByRay(ray, transform * mTransformationMatrix, 0);
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform) const noexcept {
			return intersects(ray, minDistance, maxDistance, isecInfo, transform * mTransformationMatrix);
		}
	}
}
