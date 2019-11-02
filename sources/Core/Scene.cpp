#include "Core/Scene.h"

using namespace Tachyon;
using namespace Tachyon::Core;

// I want this code to reflect closely GLSL code
using namespace glm;

constexpr glm::float32 infinity = std::numeric_limits<glm::float32>::infinity();
constexpr float minusInfinity = (-1.0) * infinity;

static const Geometry emptyGeometry = Geometry();
static const GeometryCollection emptyGeometryCollection = GeometryCollection();

Scene::Scene() noexcept : mBLAS_ModelMatrix() {
	// Initialize every BLAS ModelMatrix to the indentity matrix
	for (glm::uint32 i = 0; i < maxNumberOfBLASInTLAS; ++i) mBLAS_ModelMatrix[i] = glm::mat4(1);
}

Scene::AABB Scene::AABB::applyTransform(const AABB& starting, glm::mat4 transform) noexcept {
	vec4 v[8] = {
		transform* vec4(starting.position.x, starting.position.y, starting.position.z, 1),
		transform* vec4(starting.position.x, starting.position.y, starting.position.z + starting.dimensions.z, 1),
		transform* vec4(starting.position.x, starting.position.y + starting.dimensions.y, starting.position.z, 1),
		transform* vec4(starting.position.x, starting.position.y + starting.dimensions.y, starting.position.z + starting.dimensions.z, 1),
		transform* vec4(starting.position.x + starting.dimensions.x, starting.position.y, starting.position.z, 1),
		transform* vec4(starting.position.x + starting.dimensions.x, starting.position.y, starting.position.z + starting.dimensions.z, 1),
		transform* vec4(starting.position.x + starting.dimensions.x, starting.position.y + starting.dimensions.y, starting.position.z, 1),
		transform* vec4(starting.position.x + starting.dimensions.x, starting.position.y + starting.dimensions.y, starting.position.z + starting.dimensions.z, 1),
	};

	float maxX = minusInfinity, maxY = minusInfinity, maxZ = minusInfinity, minX = infinity, minY = infinity, minZ = infinity;

	for (uint i = 0; i < 8; ++i) {
		maxX = (v[i].x > maxX) ? v[i].x : maxX;
		maxY = (v[i].y > maxY) ? v[i].y : maxY;
		maxZ = (v[i].z > maxZ) ? v[i].z : maxZ;
		minX = (v[i].x < minX) ? v[i].x : minX;
		minY = (v[i].y < minY) ? v[i].y : minY;
		minZ = (v[i].z < minZ) ? v[i].z : minZ;
	}

	return AABB(vec4(minX, minY, minZ, 1), vec4(maxX - minX, maxY - minY, maxZ - minZ, 0));
}