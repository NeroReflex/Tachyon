#pragma once

#include "Core/RayInterceptable.h"

namespace Tachyon {
	namespace Core {

		/**
		 * This represents an Axis-Aligned Bounding Box.
		 *
		 *    +--------------+
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
		 * Position vertex
		 */
		class AABB :
			virtual public RayInterceptable {

		public:
			AABB(glm::vec3 position, NumericType length, NumericType depth, NumericType width) noexcept;

			AABB(const AABB& box1, const AABB& box2) noexcept;

			AABB(const AABB& aabb, const glm::mat4& transformationMatrix) noexcept;

			AABB(const std::vector<glm::vec3>& vertexBuffer, const glm::mat4& transformationMatrix = glm::mat4(1)) noexcept;

			AABB(const glm::vec3* vertexBuffer, size_t count, const glm::mat4& transformationMatrix = glm::mat4(1)) noexcept;

			AABB() noexcept;

			AABB(const AABB&) noexcept;

			AABB& operator=(const AABB&) noexcept;

			~AABB();

			void linearize(Tachyon::Rendering::AABB& aabb) const noexcept;

			/**
			 * Check if the given ray intersects the AABB.
			 *
			 * @return true on hit
			 */
			bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept final;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept final;

			bool contains(const glm::vec3& point) const noexcept;

			NumericType getVolume() const noexcept;

			NumericType getSurface() const noexcept;

			static NumericType SAH(const AABB& containerAABB, const AABB& containedAABB) noexcept;

			static AABB generateFromSphere(glm::vec3 centre, NumericType radius) noexcept;

			static bool areOverlapping(const AABB& box1, const AABB& box2) noexcept;

			void move(glm::vec3 vector) noexcept;

			glm::vec3 getPosition() const noexcept;

			NumericType getLength() const noexcept;

			NumericType getWidth() const noexcept;

			NumericType getDepth() const noexcept;

			/**
			 * Returns the complete list of the 8 vertices of the AABB.
			 *
			 * @return the eight vertices
			 */
			std::vector<glm::vec3> getVertices() const noexcept;

		private:
			glm::vec3 mPosition;

			NumericType mLength, mWidth, mDepth;
		};

	}
}
