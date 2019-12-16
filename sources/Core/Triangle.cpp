#include "Triangle.h"

using namespace Tachyon;
using namespace Tachyon::Core;

Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) noexcept {
	static_assert( sizeof(Triangle) == 48, "The layout is not std140 - wrong vertices size" );
	static_assert( alignof(Triangle::vertices) == 16, "The layout is not std140 - wrong vertices alignment");

	vertices[0] = glm::vec4(v1, 1);
	vertices[1] = glm::vec4(v2, 1);
	vertices[2] = glm::vec4(v3, 1);
}

Triangle::Triangle(const Triangle& src) noexcept {
	vertices[0] = vertices[0];
	vertices[1] = vertices[1];
	vertices[2] = vertices[2];
}

Triangle& Triangle::operator=(const Triangle& src) noexcept {
	if (this != &src) {
		vertices[0] = vertices[0];
		vertices[1] = vertices[1];
		vertices[2] = vertices[2];
	}

	return *this;
}
