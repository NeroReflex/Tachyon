#pragma once

#include "RayInterceptable.h"
#include "Linearizable.h"

namespace Tachyon {
	namespace Core {
		/**
		 * This class is used to store a simple geometric shape that can be intersected by a ray.
		 * As the raytracer algorithm can be executed on specilized hardware (GPU) linearization is a necessity.
		 * Each geometric shape is linearized a set of 4 vec4 components.
		 * - Empty:  vec4(NaN, NaN, NaN, NaN)
		 *           vec4(NaN, NaN, NaN, NaN)
		 *           vec4(NaN, NaN, NaN, NaN)
		 *           vec4(NaN, NaN, NaN, NaN)
		 * - Sphere: vec4(0, 0, 0, 0) <= sphere signature
		 *           vec4(center, 1.0) <= the center position
		 *           vec4(radius, 0, 0, 0) <= the radius
		 *           vec4(??, ??, ??, ??) <= UNUSED
		 *
		 * An empty shape is used as the last element of a buffer or successive geometry data (geometry collection).
		 */
		class Geometry :
			virtual public Linearizable,
			virtual public RayInterceptable {

			class LinearizableShape :
				virtual public Linearizable {

			public:
				size_t getLinearBufferSize() const noexcept final;
			};

			/**
			 * This class represents a triangle with vertices in anti-clockwise order
			 */
			class Triangle :
				virtual public Linearizable,
				virtual public LinearizableShape,
				virtual public RayInterceptable {

			public:
				Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

				Triangle(const std::array<glm::vec3, 3>& data) noexcept;

				Triangle(const Triangle& src) noexcept;

				bool isHitBy(const Ray& ray) const noexcept override;

				bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;

				void linearizeToBuffer(void* buffer) const noexcept final;

			private:
				std::array<glm::vec4, 3> mVertices;
			};

			/**
			 * This class represents a sphere
			 */
			class Sphere :
				virtual public Linearizable,
				virtual public LinearizableShape,
				virtual public RayInterceptable {

			public:
				Sphere(glm::vec3 origin, glm::float32 radius) noexcept;
				
				Sphere(const Sphere& src) noexcept;

				glm::vec3 getOrigin() const noexcept;

				glm::float32 getRadius() const noexcept;
				
				bool isHitBy(const Ray& ray) const noexcept override;

				bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;

				void linearizeToBuffer(void* buffer) const noexcept final;

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

			static size_t getMaxLinearBufferSize() noexcept;

			size_t getLinearBufferSize() const noexcept override;

			void linearizeToBuffer(void* buffer) const noexcept override;

			static void linearizeEmptyToBuffer(void* buffer) noexcept;

			bool isHitBy(const Ray& ray) const noexcept override;

			bool intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept override;

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
				struct Sphere mGeometryAsSphere;

				struct Triangle mGeometryAsTriangle;
			};
			

		};
	}
}