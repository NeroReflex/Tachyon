#include "Rendering/CPU/CPURenderer.h"

#include "PPMImage.h"

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);
	
	const auto sphereTest = Tachyon::Core::GeometryCollection({
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, 0), 0.5),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0.75, 0, -0.5), 0.25),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, -100.5, 0), 100)
	});

	Tachyon::Core::BLAS sphericBLAS;
	sphericBLAS.setTransform(glm::translate(glm::vec3(0, 0, -1)));
	sphericBLAS.insert(sphereTest);

	Tachyon::Core::RenderContext ctx(
		Tachyon::Core::Camera(
			glm::vec3(0, 0, 0), // position
			glm::vec3(0, 0, -1), // lookAt
			glm::vec3(0, 1, 0), // Up vector
			glm::float32(60.0f), // FoV in degree
			static_cast<glm::float32>(image.getWidth()) / static_cast<glm::float32>(image.getHeight()) // Aspect Ratio
		)
	);
	ctx.getRaytracingAS().insert(sphericBLAS);

	Tachyon::Rendering::CPU::CPURenderer renderer;
	renderer.render(ctx, Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader);
	renderer.transfertResult(image);

	image.write("output.ppm");

	

    return EXIT_SUCCESS;
}
