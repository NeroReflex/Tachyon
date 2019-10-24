#include "Core/AABB.h"

using namespace Tachyon;
using namespace Tachyon::Core;

AABB::AABB() noexcept
	: mPosition(glm::vec3(0,0,0)), mLength(0), mWidth(0), mDepth(0) {}

AABB::AABB(glm::vec3 position, NumericType length, NumericType depth, NumericType width) noexcept
	: mPosition(std::move(position)), mLength(std::move(length)), mWidth(std::move(width)), mDepth(std::move(depth)) {}

AABB::AABB(const AABB& box1, const AABB& box2) noexcept
: mPosition(std::move(glm::vec3(std::min(box1.mPosition.x, box2.mPosition.x), std::min(box1.mPosition.y, box2.mPosition.y), std::min(box1.mPosition.z, box2.mPosition.z)))),
	mLength(std::max(box1.mPosition.x + box1.mLength, box2.mPosition.x + box2.mLength) - mPosition.x),
	mDepth(std::max(box1.mPosition.y + box1.mDepth, box2.mPosition.y + box2.mDepth) - mPosition.y),
	mWidth(std::max(box1.mPosition.z + box1.mWidth, box2.mPosition.z + box2.mWidth) - mPosition.z) {}

AABB::AABB(const AABB& aabb, const glm::mat4& transformationMatrix) noexcept
	: AABB(aabb.getVertices(), transformationMatrix) {}

AABB::AABB(const std::vector<glm::vec3>& vertexBuffer, const glm::mat4& transformationMatrix) noexcept {
	NumericType
		minX = std::numeric_limits<glm::float64>::max(),
		maxX = std::numeric_limits<glm::float64>::min(),
		minY = std::numeric_limits<glm::float64>::max(),
		maxY = std::numeric_limits<glm::float64>::min(),
		minZ = std::numeric_limits<glm::float64>::max(),
		maxZ = std::numeric_limits<glm::float64>::min();

	for (const auto vertex : vertexBuffer) {
		const glm::vec3 vertexWorldPosition = glm::vec3(transformationMatrix * glm::vec4(vertex, 1));

		minX = std::min(vertexWorldPosition.x, minX);
		maxX = std::max(vertexWorldPosition.x, maxX);
		minY = std::min(vertexWorldPosition.y, minY);
		maxY = std::max(vertexWorldPosition.y, maxY);
		minZ = std::min(vertexWorldPosition.z, minZ);
		maxZ = std::max(vertexWorldPosition.z, maxZ);
	}

	mPosition = glm::vec3(minX, minY, minZ);
	mLength = maxX - minX;
	mDepth = maxY - minY;
	mWidth = maxZ - minZ;
}

AABB::AABB(const AABB& src) noexcept 
	: mPosition(src.mPosition), mLength(src.mLength), mWidth(src.mWidth), mDepth(src.mDepth) {}

AABB& AABB::operator=(const AABB& src) noexcept {
	mPosition = src.mPosition;
	mLength = src.mLength;
	mWidth = src.mWidth;
	mDepth = src.mDepth;

	return *this;
}

AABB::~AABB() = default;

NumericType AABB::getVolume() const noexcept {
	return mLength * mWidth * mDepth;
}

NumericType AABB::getSurface() const noexcept {
	return ((mLength * mWidth) * 2) + ((mWidth * mDepth) * 2) + ((mLength * mDepth) * 2);
}

NumericType AABB::SAH(const AABB& containerAABB, const AABB& containedAABB) noexcept {
	return containedAABB.getSurface() / containerAABB.getSurface();
}

bool AABB::contains(const glm::vec3& point) const noexcept {
	return ((point.x >= mPosition.x) && (point.x <= (mPosition.x + mLength))) &&
		((point.y >= mPosition.y) && (point.y <= (mPosition.y + mDepth))) &&
		((point.z >= mPosition.z) && (point.z <= (mPosition.z + mWidth)));
}

AABB AABB::generateFromSphere(glm::vec3 centre, NumericType radius) noexcept {
	AABB box;
	box = AABB(glm::vec3(centre.x - radius, centre.y - radius, centre.z - radius), 2 * radius, 2 * radius, 2 * radius);

	return box;
}

bool AABB::areOverlapping(const AABB& box1, const AABB& box2) noexcept {
	const glm::vec3 basePosition = box2.mPosition;

	const std::array<glm::vec3, 8> verticesToCheck = {
		basePosition,
		glm::vec3(basePosition.x, basePosition.y, basePosition.z + box2.mWidth),
		glm::vec3(basePosition.x, basePosition.y + box2.mDepth, basePosition.z),
		glm::vec3(basePosition.x, basePosition.y + box2.mDepth, basePosition.z + box2.mWidth),
		glm::vec3(basePosition.x + box2.mLength, basePosition.y, basePosition.z),
		glm::vec3(basePosition.x + box2.mLength, basePosition.y, basePosition.z + box2.mWidth),
		glm::vec3(basePosition.x + box2.mLength, basePosition.y + box2.mDepth, basePosition.z),
		glm::vec3(basePosition.x + box2.mLength, basePosition.y + box2.mDepth, basePosition.z + box2.mWidth),
	};

	// Two Axis-Aligned boxes are overlapping if one of the vertex of the first box is inside the second box
	return
		(box1.contains(verticesToCheck[0])) ||
		(box1.contains(verticesToCheck[1])) ||
		(box1.contains(verticesToCheck[2])) ||
		(box1.contains(verticesToCheck[3])) ||
		(box1.contains(verticesToCheck[4])) ||
		(box1.contains(verticesToCheck[5])) ||
		(box1.contains(verticesToCheck[6])) ||
		(box1.contains(verticesToCheck[7]));
}

glm::vec3 AABB::getPosition() const noexcept {
	return mPosition;
}

NumericType AABB::getLength() const noexcept {
	return mLength;
}

NumericType AABB::getWidth() const noexcept {
	return mWidth;
}

NumericType AABB::getDepth() const noexcept {
	return mDepth;
}

void AABB::move(glm::vec3 vector) noexcept {
	mPosition += vector;
}

std::vector<glm::vec3> AABB::getVertices() const noexcept {
	const std::vector<glm::vec3> vertices = {
		mPosition,
		glm::vec3(mPosition.x, mPosition.y, mPosition.z + mWidth),
		glm::vec3(mPosition.x, mPosition.y + mDepth, mPosition.z),
		glm::vec3(mPosition.x, mPosition.y + mDepth, mPosition.z + mWidth),
		glm::vec3(mPosition.x + mLength, mPosition.y, mPosition.z),
		glm::vec3(mPosition.x + mLength, mPosition.y, mPosition.z + mWidth),
		glm::vec3(mPosition.x + mLength, mPosition.y + mDepth, mPosition.z),
		glm::vec3(mPosition.x + mLength, mPosition.y + mDepth, mPosition.z + mWidth),
	};

	return vertices;
}

bool AABB::intersection(const Ray& ray, glm::float32 minDistance, glm::float32 maxDistance, RayGeometryIntersection& isecInfo) const noexcept {
    return isHitBy(ray);
}

bool AABB::isHitBy(const Ray& ray) const noexcept {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	glm::vec3 orig = ray.getOrigin();
	glm::vec3 invdir = ray.getInvDirection();

	std::array<glm::vec3, 2> bounds = {
		mPosition,
		glm::vec3(mPosition.x + mLength, mPosition.y + mDepth, mPosition.z + mWidth)
	};

	tmin = (bounds[ray.getSigns()[0]].x - orig.x) * invdir.x;
	tmax = (bounds[1 - ray.getSigns()[0]].x - orig.x) * invdir.x;
	tymin = (bounds[ray.getSigns()[1]].y - orig.y) * invdir.y;
	tymax = (bounds[1 - ray.getSigns()[1]].y - orig.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[ray.getSigns()[2]].z - orig.z) * invdir.z;
	tzmax = (bounds[1 - ray.getSigns()[2]].z - orig.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}