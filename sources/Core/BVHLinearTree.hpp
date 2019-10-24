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
			BVHLinearTree(const BVHLinearTree<ContentType, N>&) = delete;
			BVHLinearTree(BVHLinearTree&&) = delete;
			BVHLinearTree& operator=(const BVHLinearTree<ContentType, N>&) = delete;
			virtual ~BVHLinearTree();

			BVHLinearTree() noexcept;

			/**
			 * Insert an element into the BVH tree.
			 */
			bool insert(const ContentType& uniqueInstance) noexcept;

			/**
			 * Traverse each node of the tree to execute the given function on each leaf.
			 */
			void traverse(const std::function<void(const ContentType&)>& fn, UnsignedType root) const noexcept;

			bool isHitBy(const Ray& ray) const noexcept final;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept final;

		private:
			/**
			 * Keeps track of free nodes
			 */
			std::list<glm::uint64> mFreeNodes;

		protected:
			bool isRoot(UnsignedType index) const noexcept;

			bool isLeaf(UnsignedType index) const noexcept;

			AABB& bv(UnsignedType index) noexcept;

			const AABB& bv(UnsignedType index) const noexcept;

			bool isFree(UnsignedType index) const noexcept;

			bool isBVHitByRay(const Ray& ray, UnsignedType root = 0) const noexcept;

			bool intersects(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, UnsignedType root = 0) const noexcept;

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
			};

			UnsignedType insert(NodeData node, UnsignedType root = 0) noexcept;

			Collection<ContentType, N> mContentCollection;
			std::array<NodeData, maxNumberOfTreeElements> mLinearTree;

		};

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::BVHLinearTree() noexcept {
			for (size_t i = 0; i < BVHLinearTree::maxNumberOfTreeElements; ++i)
				mFreeNodes.emplace_back(i);
		}

		template <class ContentType, size_t N>
		BVHLinearTree<ContentType, N>::~BVHLinearTree() {}

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
		bool BVHLinearTree<ContentType, N>::insert(const ContentType& element) noexcept {
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

			// Susccess!
			return true;
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
		bool BVHLinearTree<ContentType, N>::isBVHitByRay(const Ray& ray, UnsignedType root = 0) const noexcept {
			if (isFree(root)) return false;

			if (isLeaf(root)) {
				return mContentCollection[mLinearTree[root].content].isHitBy(ray);
			} else if (bv(root).isHitBy(ray)) {
				bool leftHit = false, rightHit = false;

				if (bv(mLinearTree[root].tree.mLeft).isHitBy(ray)) {
					leftHit = isBVHitByRay(ray, mLinearTree[root].tree.mLeft);
				}

				if (bv(mLinearTree[root].tree.mRight).isHitBy(ray)) {
					rightHit = isBVHitByRay(ray, mLinearTree[root].tree.mRight);
				}

				return leftHit || rightHit;
			}

			return false;
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::intersects(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, UnsignedType root = 0) const noexcept {
			if (isFree(root)) return false;

			bool match = false;

			if (isLeaf(root)) {
				return mContentCollection[mLinearTree[root].content].intersection(ray, minDistance, maxDistance, isecInfo);
			} else if (bv(root).isHitBy(ray)) {
				bool leftHit = false, rightHit = false;

				if (bv(mLinearTree[root].tree.mLeft).isHitBy(ray)) {
					leftHit = intersects(ray, minDistance, maxDistance, isecInfo, mLinearTree[root].tree.mLeft);
				}

				if (bv(mLinearTree[root].tree.mRight).isHitBy(ray)) {
					RayGeometryIntersection temp;
					rightHit = intersects(ray, minDistance, maxDistance, temp, mLinearTree[root].tree.mRight);

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
		bool BVHLinearTree<ContentType, N>::isHitBy(const Ray& ray) const noexcept {
			return isBVHitByRay(ray, 0);
		}

		template <class ContentType, size_t N>
		bool BVHLinearTree<ContentType, N>::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept {
			return intersects(ray, minDistance, maxDistance, isecInfo);
		}
	}
}
