#include "Rendering/Renderer.h"
#include "Rendering/ToneMapping/ExposureToneMapper.h"

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);

	Tachyon::Rendering::RenderSurface renderTarget(200, 100);

	Tachyon::Rendering::Renderer renderer(renderTarget);
	renderer.render(
		Tachyon::Core::GeometryCollection({
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, -1), 0.5),
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0,-100.5,-1), 100)
		})
	);
	renderTarget.transferTo(image, Tachyon::Rendering::ToneMapping::ExposureToneMapper(0.1));

	image.write("output.ppm");

    return EXIT_SUCCESS;
}
