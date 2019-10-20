#pragma once

#include "RayInterceptable.h"

namespace Tachyon {
	namespace Core {
		class Geometry :
			virtual public RayInterceptable {

			class Triangle :
				virtual public RayInterceptable {

			public:
				Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept;

				Triangle(const std::array<glm::vec3, 3>& data) noexcept;

				Triangle(const Triangle& src) noexcept;

				bool isHitBy(const Ray& ray) const noexcept override;

			private:
				std::array<glm::vec4, 3> mVertices;
			};

			class Sphere :
				virtual public RayInterceptable {

			public:
				Sphere(glm::vec3 origin, glm::float32 radius) noexcept;
				
				Sphere(const Sphere& src) noexcept;

				glm::vec3 getOrigin() const noexcept;

				glm::float32 getRadius() const noexcept;
				
				bool isHitBy(const Ray& ray) const noexcept override;

			private:
				glm::vec4 mOrigin;
				glm::float32 mRadius;
			};

		public:
			enum Type {
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

			bool isHitBy(const Ray& ray) const noexcept override;

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