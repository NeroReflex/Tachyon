#pragma once

#include "RayInterceptable.h"
#include "Core/AABB.h"

namespace Tachyon {
	namespace Core {

		/**
		 * This class is used to store a simple geometric shape that can be intersected by a ray.
		 * As the raytracer algorithm can be executed on specilized hardware (GPU) linearization is a necessity.
		 * Each geometric shape is linearized a set of 4 vec4 components.
		 * - Sphere:   vec4(0, 0, 0, 0) <= sphere signature
		 *             vec4(center, 1.0) <= the center position
		 *             vec4(radius, 0, 0, 0) <= the radius
		 *             vec4(??, ??, ??, ??) <= UNUSED
		 * - Triangle: vec4(0, 0, 0, 1) <= triangle signature
		 *             vec4(v0.x, v0.y, v0.z, 1)
		 *             vec4(v1.x, v1.y, v1.z, 1)
		 *             vec4(v2.x, v2.y, v2.z, 1)
		 * An empty shape is used as the last element of a buffer or successive geometry data (geometry collection).
		 */
		class Geometry :
			virtual public RayInterceptable {

			/**
			 * This class represents a triangle with vertices in anti-clockwise order
			 */
			class Triangle :
				virtual public RayInterceptable {

			public:
				Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

				Triangle(const std::array<glm::vec3, 3>& data) noexcept;

				Triangle(const Triangle& src) noexcept;
				
				bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept final;

				bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept final;
				
				static void linearizeToVec4(const Triangle& src, glm::vec4* dst) noexcept {
					size_t i = 0;
					std::for_each(src.mVertices.cbegin(), src.mVertices.cend(), [&dst, &i](const glm::vec4& vertex) {
						dst[i++] = vertex;
					});
				}

				void linearize(Tachyon::Rendering::Geometry& geometry) const noexcept {
					geometry.vertex1_or_sphere_center = mVertices[0];
					geometry.vertex2_or_sphere_radius = mVertices[1];
					geometry.vertex3_or_nothing = mVertices[2];
				}

			private:
				std::array<glm::vec4, 3> mVertices;
			};

			/**
			 * This class represents a sphere
			 */
			class Sphere :
				virtual public RayInterceptable {

			public:
				Sphere(glm::vec3 origin, glm::float32 radius) noexcept;
				
				Sphere(const Sphere& src) noexcept;

				glm::vec3 getOrigin() const noexcept;

				glm::float32 getRadius() const noexcept;
				
				bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept final;

				bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept final;

				static void linearizeToVec4(const Sphere& src, glm::vec4* dst) noexcept {
					dst[0] = src. mOrigin;
					dst[1].x = src.mRadius;
				}

				void linearize(Tachyon::Rendering::Geometry& geometry) const noexcept {
					geometry.vertex1_or_sphere_center = mOrigin;
					geometry.vertex2_or_sphere_radius = glm::vec4(mRadius, 0, 0, 0);
					geometry.vertex3_or_nothing = glm::vec4(0, 0, 0, 0);
				}

			private:
				glm::vec4 mOrigin;
				glm::float32 mRadius;
			};

		public:
			enum class Type {
				Sphere = 0,
				Triangle = 1,

			};

			/**
			 * Default constructor:
			 * Create a new geometry as a sphere with origin at the center of the world vec3(0,0,0) and radius 1. 
			 */
			Geometry() noexcept;

			Geometry(const Geometry& src) noexcept;

			Geometry& operator=(const Geometry& src) noexcept;

			static Geometry makeSphere(glm::vec3 origin, glm::float32 radius) noexcept;

			static Geometry makeTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

			constexpr static size_t linearSizeInVec4() noexcept {
				return 4;
			}

			static void linearizeToVec4(const Geometry& src, glm::vec4* destination) noexcept {
				if (src.mType == Type::Sphere) {
					// This is the sphere signature
					destination[0] = glm::vec4(0, 0, 0, 0);

					Sphere::linearizeToVec4(src.mGeometryAsSphere, &destination[1]);
				} else if (src.mType == Type::Triangle) {
					// This is the triangle signature
					destination[0] = glm::vec4(0, 0, 0, 1);

					Triangle::linearizeToVec4(src.mGeometryAsTriangle, &destination[1]);
				}
			}

			bool isHitBy(const Ray& ray, glm::mat4 transform = glm::mat4(1)) const noexcept override;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform = glm::mat4(1)) const noexcept override;

			AABB bvBase() const noexcept;

			void linearize(Tachyon::Rendering::Geometry& geometry) const noexcept;

		protected:
			/**
			 * Create a new geometry as as sphere
			 */
			Geometry(glm::vec3 position, glm::float32 radius) noexcept;

			/**
			 * Create a new geometry as as triangle
			 */
			Geometry(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

			/**
			 * Create a new geometry as as triangle
			 */
			Geometry(const std::array<glm::vec3, 3>& data) noexcept;

		private:
			Type mType;
			
			union {
				Sphere mGeometryAsSphere;

				Triangle mGeometryAsTriangle;
			};
			

		};

	}
}