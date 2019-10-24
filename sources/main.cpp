#include "Rendering/Renderer.h"
#include "Rendering/ToneMapping/ExposureToneMapper.h"

#include "Core/BLAS.h"

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);

	Tachyon::Rendering::RenderSurface renderTarget(480, 360);
	const auto aspectRatio = static_cast<glm::float32>(renderTarget.getWidth()) / static_cast<glm::float32>(renderTarget.getHeight());
	Tachyon::Core::Camera renderCamera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::float32(60.0f), aspectRatio);

	Tachyon::Rendering::Renderer renderer(renderTarget);
	const auto sphereTest = Tachyon::Core::GeometryCollection({
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, 0), 0.5),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0.75, 0, -0.5), 0.25),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, -100.5, 0), 100)
	});

	Tachyon::Core::BLAS sphericBLAS;
	sphericBLAS.setTransform(glm::translate(glm::vec3(0, 0, -1)));
	sphericBLAS.insert(sphereTest);

	// The TLAS occupy a big chunk of memory, therefore it is advised it stays on the heap
	std::unique_ptr<Tachyon::Core::TLAS> scene(new Tachyon::Core::TLAS());
	scene->insert(sphericBLAS);

	Tachyon::Rendering::ToneMapping::ExposureToneMapper toneMapper(0.1);

	renderer.render(renderCamera, Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader, *scene);
	renderTarget.transferTo(image, toneMapper);

	image.write("output.ppm");

    return EXIT_SUCCESS;
}
