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

bool Geometry::Triangle::isHitBy(const Ray& ray, glm::mat4 transform) const noexcept {
	return false;
}

bool Geometry::Triangle::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform) const noexcept {
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

bool Geometry::Sphere::isHitBy(const Ray& ray, glm::mat4 transform) const noexcept {
	const glm::vec3 origin = transform * mOrigin;
	const auto radius = this->getRadius();

	if (radius == 0.0) return false;

	glm::vec3 oc = ray.getOrigin() - this->getOrigin();
	
	const auto a = glm::dot(ray.getDirection(), ray.getDirection());
	const auto b = glm::dot(oc, ray.getDirection());
	const auto c = glm::dot(oc, oc) - radius * radius;
	const auto discriminant = b * b - a * c;

	return discriminant > 0.0;
}

bool Geometry::Sphere::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform) const noexcept {
	const glm::vec3 center = transform * mOrigin;
	const auto radius = this->getRadius();
	const auto direction = ray.getDirection();

	if (radius == 0.0) return false;

	glm::vec3 oc = ray.getOrigin() - center;
	
	const auto a = glm::dot(direction, direction);
	const auto b = glm::dot(oc, direction);
	const auto c = glm::dot(oc, oc) - radius * radius;
	const auto discriminant = b * b - a * c;

	if (discriminant > 0.0) { // if delta > 0 then we have two intersections (one for each side of the sphere)
		const glm::float32 squareRoot = glm::sqrt(discriminant);

		// x0 and x1 are the distances to the origin of the ray
		glm::float32 x0 = (-b - squareRoot) / a, x1 = (-b + squareRoot) / a;

		// Use x0 and x1 to calculate intersection points
		glm::vec3 point_x0 = ray.pointAt(x0), point_x1 = ray.pointAt(x1); // so if I use that distance as a coefficient I obtain the intersection point
		
		// Use intersecting points to calculate the surface normal at that point
		glm::vec3 normal_x0 = (point_x0 - center) / radius, normal_x1 = (point_x1 - center) / radius; // and the normal for a point p is (point - center)/radius

		// No valid intersection? return false.
		if (((x0 <= minDistance) || (x0 >= maxDistance)) && ((x1 <= minDistance) || (x1 >= maxDistance))) return false;

		// Choose the closest intersection point
		isecInfo = (((x0 > minDistance) && (x0 < maxDistance)) && (x0 <= x1)) || ((x1 <= minDistance) || (x1 >= maxDistance)) ?
			RayGeometryIntersection(point_x0, x0, normal_x0) : RayGeometryIntersection(point_x1, x1, normal_x1);

		return true;
	}

	return false;
}

Geometry Geometry::makeSphere(glm::vec3 origin, glm::float32 radius) noexcept {
	return Geometry(origin, radius);
}

Geometry Geometry::makeTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept {
	return Geometry(std::move(v1), std::move(v2), std::move(v3));
}

Geometry::Geometry() noexcept
	: mType(Geometry::Type::Sphere),
	mGeometryAsSphere(glm::vec3(0, 0, 0), 0) {}

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

bool Geometry::isHitBy(const Ray& ray, glm::mat4 transform) const noexcept {
	if (mType == Geometry::Type::Sphere)
		return this->mGeometryAsSphere.isHitBy(ray, transform);
	else if (mType == Geometry::Type::Triangle)
		return this->mGeometryAsTriangle.isHitBy(ray, transform);
	else // unknown geometry type
		return false;
}

bool Geometry::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo, glm::mat4 transform) const noexcept {
	if (mType == Geometry::Type::Sphere)
		return this->mGeometryAsSphere.intersection(ray, minDistance, maxDistance, isecInfo, transform);
	else if (mType == Geometry::Type::Triangle)
		return this->mGeometryAsTriangle.intersection(ray, minDistance, maxDistance, isecInfo, transform);
	else // unknown geometry type
		return false;
}

AABB Geometry::bvBase() const noexcept
{
	if (mType == Geometry::Type::Sphere) {
		const auto origin = mGeometryAsSphere.getOrigin();
		const auto radius = mGeometryAsSphere.getRadius();

		return AABB(std::vector<glm::vec3>({
			glm::vec3(origin.x - radius, origin.y - radius, origin.z - radius),
			glm::vec3(origin.x + radius, origin.y + radius, origin.z + radius)
		}));
	}

	DBG_ASSERT(false);

	return AABB();
}
