#include "Rendering/CPU/CPURenderer.h"
#include "Rendering/OpenGL/OpenGLRenderer.h"

#include "PPMImage.h"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

std::unordered_map<uintptr_t, Tachyon::Rendering::Renderer*> windowToRenderer;

std::string get_opengl_compute_info() {
	std::ostringstream stringstream;

	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	stringstream << "max global (total) work group size x:" << work_grp_cnt[0] << " y:" << work_grp_cnt[1] << " z:" << work_grp_cnt[2] << std::endl;

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	stringstream << "max local (in one shader) work group sizes x:" << work_grp_size[0] << " y:" << work_grp_size[1]  << " z:" << work_grp_size[2] << std::endl;

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	stringstream << "max local work group invocations " << work_grp_inv << std::endl;

	return stringstream.str();
}

void window_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char** argv) {
	Tachyon::PPMImage image(480, 360);
	
	const auto sphereTest = Tachyon::Core::GeometryCollection({
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, 0, 0), 0.5),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0.75, 0, -0.5), 0.25),
		Tachyon::Core::Geometry::makeSphere(glm::vec3(0, -100.5, 0), 100)
	});

	std::unique_ptr<Tachyon::Core::BLAS> sphericBLAS(new Tachyon::Core::BLAS());
	sphericBLAS->setTransform(glm::translate(glm::vec3(0, 0, -1)));

	// TODO: try to enable this!
	//sphericBLAS->insert(Tachyon::Core::GeometryCollection());
	sphericBLAS->insert(sphereTest);

	Tachyon::Core::RenderContext ctx(
		Tachyon::Core::Camera(
			glm::vec3(0, 0, 0), // position
			glm::vec3(0, 0, -1), // lookAt
			glm::vec3(0, 1, 0), // Up vector
			glm::float32(60.0f), // FoV in degree
			static_cast<glm::float32>(image.getWidth()) / static_cast<glm::float32>(image.getHeight()) // Aspect Ratio
		)
	);
	ctx.getRaytracingAS().insert(*sphericBLAS);

	// Initialize GLFW
	if (glfwInit() == 0) {
		std::cout << "Error: cannot initialize GLFW" << std::endl;

		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// TODO: let the user decide the input antialiasing
	glfwWindowHint(GLFW_SAMPLES, 16);

	// TODO: let the user specify preferred resolution
	GLFWwindow* window = glfwCreateWindow(480, 360, "Tachyon Raytracer", nullptr, nullptr);

	if (!window) {
		std::cout << "Error: cannot open a window" << std::endl;

		return EXIT_FAILURE;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (gl3wInit()) {
		std::cout << "Error: cannot initialize OpenGL context" << std::endl;

		return EXIT_FAILURE;
	}

	if (!gl3wIsSupported(4, 5)) {
		std::cout << "Error: OpenGL 4.5 is required to run this application. Please, update your video driver!" << std::endl;

		return EXIT_FAILURE;
	}

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Retrieve the initial window size to initialize the renderer
	int initialWidth, initialHeight;
	glfwGetWindowSize(window, &initialWidth, &initialHeight);

	// Now it is safe to create the renderer
	std::unique_ptr<Tachyon::Rendering::OpenGL::OpenGLRenderer> raytracer(new Tachyon::Rendering::OpenGL::OpenGLRenderer(ctx, initialWidth, initialHeight));

	// Register the current windows <=> renderer association
	windowToRenderer.emplace(std::make_pair<uintptr_t, Tachyon::Rendering::Renderer*>((uintptr_t)window, raytracer.get()));

	// Register the window resize callback
	glfwSetWindowSizeCallback(window, window_size_callback);

	std::string rendererId =
		std::string("OpenGL v") +
		std::string((const char*)glGetString(GL_VERSION)) +
		std::string(", GLSL ") +
		std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	std::cout << "Rendering with: " << rendererId << std::endl << get_opengl_compute_info() << std::endl;

	int uboMaxSize = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboMaxSize);

	std::cout << "Max UBO length in bytes: " << uboMaxSize << std::endl;

	int textureDimensionMaxSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureDimensionMaxSize);

	std::cout << "Max texture dimension: " << textureDimensionMaxSize << std::endl;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		raytracer->render(Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader);

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
	Tachyon::Rendering::CPU::CPURenderer renderer(ctx, 480, 360);
	renderer.render(Tachyon::Rendering::Renderer::ShaderAlgorithm::DistanceShader);
	renderer.transfertResult(image);
	image.write("output.ppm");
	*/

    return EXIT_SUCCESS;
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
	const auto rendererCit = windowToRenderer.find((uintptr_t)window);

	DBG_ASSERT( (rendererCit != windowToRenderer.cend()) );

	if (rendererCit != windowToRenderer.cend())
		rendererCit->second->resize(width, height);
	
}