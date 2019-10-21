#include "Rendering/Renderer.h"
#include "Rendering/ToneMapping/ExposureToneMapper.h"

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);

	Tachyon::Rendering::RenderSurface renderTarget(480, 360);
	const auto aspectRatio = static_cast<glm::float32>(renderTarget.getWidth()) / static_cast<glm::float32>(renderTarget.getHeight());
	Tachyon::Core::Camera renderCamera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::float32(60.0f), aspectRatio);
	

	Tachyon::Rendering::Renderer renderer(renderTarget);
	const auto scene = Tachyon::Core::GeometryCollection({
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, -1), 0.5),
			Tachyon::Core::Geometry::makeSphere(glm::vec3(0,-100.5,-1), 100)
		});

	renderer.render(renderCamera, Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader, scene);
	renderTarget.transferTo(image, Tachyon::Rendering::ToneMapping::ExposureToneMapper(0.1));

	image.write("output.ppm");

    return EXIT_SUCCESS;
}
