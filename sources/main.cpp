#include "Rendering/Renderer.h"

void editLeftCorner(Tachyon::PPMImage& renderTarget);

int main(int argc, char** argv) {
	Tachyon::PPMImage renderTarget(480, 360);

	Tachyon::Rendering::Renderer renderer(renderTarget);
	renderer.render(
		Tachyon::Core::GeometryCollection({
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, -1), 0.5)
		})
	);
	editLeftCorner(renderTarget);

	renderTarget.write("output.ppm");

    return EXIT_SUCCESS;
}

void editLeftCorner(Tachyon::PPMImage& renderTarget) {
	renderTarget.setPixel(0, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(0, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(1, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(2, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 6, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 0, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 1, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 2, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 3, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 4, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 5, glm::vec3(1, 1, 1));
	renderTarget.setPixel(3, 6, glm::vec3(1, 1, 1));
}