#include "DistanceShader.h"

using namespace Tachyon;
using namespace Tachyon::Rendering::CPU;
using namespace Tachyon::Rendering::CPU::Shading;

glm::vec3 DistanceShader::operator()(const Core::RayGeometryIntersection& isect) const noexcept {
	// Here the camera position is vec3(0, 0, 0) because all of this is in camera space
	return glm::vec3(std::max<glm::float32>(0, glm::dot(isect.getNormal(), glm::normalize(/*camera.getCameraPosition()*/ -isect.getPoint()))));
}