#include "Rendering/Renderer.h"

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);

	Tachyon::Rendering::RenderSurface renderTarget(480, 360);

	Tachyon::Rendering::Renderer renderer(renderTarget);
	renderer.render(
		Tachyon::Core::GeometryCollection({
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, -1), 0.5)
		})
	);
	renderTarget.transferTo(image);

	image.write("output.ppm");

    return EXIT_SUCCESS;
}
