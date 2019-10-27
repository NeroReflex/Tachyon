#include "Rendering/CPU/CPURenderer.h"
#include "Rendering/OpenGL/OpenGLRenderer.h"

#include "PPMImage.h"

std::unordered_map<uintptr_t, Tachyon::Rendering::Renderer*> windowToRenderer;

void window_size_callback(GLFWwindow* window, int width, int height);

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

	// OpenGL Raytracing
	DBG_ASSERT( (glfwInit() != 0) );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// TODO: let the user decide the input antialiasing
	glfwWindowHint(GLFW_SAMPLES, 16);

	GLFWwindow* window = glfwCreateWindow(480, 360, "Tachyon Raytracer", nullptr, nullptr);

	DBG_ASSERT( (window != nullptr) );

	// Make the window's context current
	glfwMakeContextCurrent(window);

	assert( (!gl3wInit()) );

	DBG_ASSERT( (gl3wIsSupported(4, 5)) );

	// Retrieve the initial window size to initialize the renderer
	int initialWidth, initialHeight;
	glfwGetWindowSize(window, &initialWidth, &initialHeight);

	// Now it is safe to create the renderer
	std::unique_ptr<Tachyon::Rendering::OpenGL::OpenGLRenderer> raytracer(new Tachyon::Rendering::OpenGL::OpenGLRenderer(initialWidth, initialHeight));

	// Register the current windows <=> renderer association
	windowToRenderer.emplace(std::make_pair<uintptr_t, Tachyon::Rendering::Renderer*>((uintptr_t)window, raytracer.get()));

	// Register the window resize callback
	glfwSetWindowSizeCallback(window, window_size_callback);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		raytracer->render(ctx, Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader);

		glfwSwapBuffers(window);
	}

	// Destroy the renderer and remove each reference to it
	raytracer.reset();
	windowToRenderer.clear();

	// Destroy the renderer surface
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	/*
	// Execute raytracing on the CPU
	Tachyon::Rendering::CPU::CPURenderer renderer(480, 360);
	renderer.render(ctx, Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader);
	renderer.transfertResult(image);
	image.write("output.ppm");
	*/

    return EXIT_SUCCESS;
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
	const auto rendererCit = windowToRenderer.find((uintptr_t)window);

	if (rendererCit != windowToRenderer.cend()) {
		rendererCit->second->resize(width, height);
	}
}