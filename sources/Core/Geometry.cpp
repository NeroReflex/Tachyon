#include "Geometry.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Geometry::Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept 
	: mVertices({
		glm::vec4(std::move(v1), 1),
		glm::vec4(std::move(v2), 1),
		glm::vec4(std::move(v3), 1)
	}) {}

Geometry::Triangle::Triangle(const std::array<glm::vec3, 3>& data) noexcept
	: mVertices({
		glm::vec4(data[0], 1),
		glm::vec4(data[1], 1),
		glm::vec4(data[2], 1)
	}) {}

Geometry::Triangle::Triangle(const Triangle& src) noexcept 
	: mVertices(src.mVertices) {}

bool Geometry::Triangle::isHitBy(const Ray& ray) const noexcept {
	return false;
}

Geometry::Sphere::Sphere(glm::vec3 origin, glm::float32 radius) noexcept
	: mOrigin(glm::vec4(std::move(origin), 1)), mRadius(std::move(radius)) {}

Geometry::Sphere::Sphere(const Sphere& src) noexcept
	: mOrigin(src.mOrigin), mRadius(src.mRadius) {}

glm::vec3 Geometry::Sphere::getOrigin() const noexcept {
	return mOrigin;
}

glm::float32 Geometry::Sphere::getRadius() const noexcept {
	return mRadius;
}

bool Geometry::Sphere::isHitBy(const Ray& ray) const noexcept {
	glm::vec3 oc = ray.getOrigin() - this->getOrigin();
	const auto radius = this->getRadius();
	const auto a = glm::dot(ray.getDirection(), ray.getDirection());
	const auto b = 2.0 * glm::dot(oc, ray.getDirection());
	const auto c = glm::dot(oc, oc) - radius * radius;
	const auto discriminant = b * b - 4.0 * a * c;

	return discriminant >= 0.0;
}

Geometry Geometry::makeSphere(glm::vec3 origin, glm::float32 radius) noexcept {
	return Geometry(origin, radius);
}

Geometry Geometry::makeTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept {
	return Geometry(std::move(v1), std::move(v2), std::move(v3));
}

Geometry::Geometry() noexcept
	: mType(Geometry::Type::Sphere),
	mGeometryAsSphere(glm::vec3(0, 0, 0), 1) {}

Geometry::Geometry(const Geometry& src) noexcept
	: mType(src.mType), mGeometryAsTriangle(src.mGeometryAsTriangle) {} // I do copy the geometry as a triangle because it is the largest data buffer :)

Geometry& Geometry::operator=(const Geometry& src) noexcept {
	mType = src.mType;
	mGeometryAsTriangle = src.mGeometryAsTriangle;

	return *this;
}

/**
 * Create a new geometry as as triangle
 */
Geometry::Geometry(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept
	: mType(Geometry::Type::Triangle),
	mGeometryAsTriangle(std::move(v1), std::move(v2), std::move(v3)) {}

/**
 * Create a new geometry as as triangle
 */
Geometry::Geometry(const std::array<glm::vec3, 3>& data) noexcept
	: mType(Geometry::Type::Triangle),
	mGeometryAsTriangle(data) {}

Geometry::Geometry(glm::vec3 position, glm::float32 radius) noexcept 
	: mType(Geometry::Type::Sphere),
	mGeometryAsSphere(std::move(position), std::move(radius)) {}

bool Geometry::isHitBy(const Ray& ray) const noexcept {
	if (mType == Geometry::Type::Sphere)
		return this->mGeometryAsSphere.isHitBy(ray);
	else if (mType == Geometry::Type::Triangle)
		return this->mGeometryAsTriangle.isHitBy(ray);
	else // unknown geometry type
		return false;
}