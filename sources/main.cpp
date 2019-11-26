#include "Core/Camera.h"

#if defined(OPENGL_SUPPORT)
	#include "Rendering/OpenGL/OpenGLPipeline.h"
#elif defined(VULKAN_SUPPORT)
	#include "Rendering/Vulkan/VulkanPipeline.h"
#endif

#if defined(OPENGL_SUPPORT)
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
#endif

#include "Memory/UnsafeAllocator.hpp"

int main(int argc, char** argv) {
	void* memoryBuffer = calloc(global_heap_block_count, Tachyon::Memory::atomicMemoryBlockSize);
	Tachyon::Memory::UnsafeAllocator<void> alloc(std::move(Tachyon::Memory::UnsafePoolManager(global_heap_block_count - 1, Tachyon::Memory::blockAlign(memoryBuffer))));

	std::vector<int, Tachyon::Memory::UnsafeAllocator<int>> testVec(700, alloc);

	// Initialize GLFW
	if (glfwInit() == 0) {
		std::cout << "Error: cannot initialize GLFW" << std::endl;

		return EXIT_FAILURE;
	}

#if defined(VULKAN_SUPPORT)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#elif defined(OPENGL_SUPPORT)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// TODO: let the user decide the input antialiasing
	glfwWindowHint(GLFW_SAMPLES, 16);
#endif

	// TODO: let the user specify preferred resolution
	GLFWwindow* window = glfwCreateWindow(480, 360, "Tachyon Raytracer", nullptr, nullptr);

	if (!window) {
		std::cout << "Error: cannot open a window" << std::endl;

		return EXIT_FAILURE;
	}

	// Now it is safe to create the renderer
	std::unique_ptr<Tachyon::Rendering::RenderingPipeline> raytracer = nullptr;
#if defined(VULKAN_SUPPORT)
	if (!glfwVulkanSupported()) {
		std::cerr << "Error: Vulkan unavailable, cannot initialize the renderer." << std::endl;

		return EXIT_FAILURE;
	}

	raytracer.reset(new Tachyon::Rendering::Vulkan::VulkanPipeline(window));
#elif defined(OPENGL_SUPPORT)

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (gl3wInit()) {
		std::cerr << "Error: cannot initialize OpenGL context" << std::endl;

		return EXIT_FAILURE;
	}

	if (!gl3wIsSupported(4, 5)) {
		std::cerr << "Error: OpenGL 4.5 is required to run this application. Please, update your video driver!" << std::endl;

		return EXIT_FAILURE;
	}

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	std::string rendererId =
		std::string("OpenGL v") +
		std::string((const char*)glGetString(GL_VERSION)) +
		std::string(", GLSL ") +
		std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	std::cout << "Rendering with: " << rendererId << std::endl << get_opengl_compute_info() << std::endl;

	int uboMaxSize = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboMaxSize);

	std::cout << "Max UBO length in bytes: " << uboMaxSize << std::endl;

	int texture2DimensionMaxSize = 0, texture3DimensionMaxSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture2DimensionMaxSize);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &texture3DimensionMaxSize);

	std::cout << "Max 2D texture dimension: " << texture2DimensionMaxSize << "x" << texture2DimensionMaxSize << std::endl;
	std::cout << "Max 3D texture dimensions:" << texture3DimensionMaxSize << "x" << texture3DimensionMaxSize << "x" << texture3DimensionMaxSize << std::endl;

	int maxBlockSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBlockSize);

	std::cout << "Max SSBO Block size: " << maxBlockSize << std::endl;

	raytracer.reset(new Tachyon::Rendering::OpenGL::OpenGLPipeline(window));
#endif

	if (!raytracer) {
		std::cerr << "Error: unable to create the renderer for an unknown reason." << std::endl;
	}
	
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	Tachyon::Core::Camera camera(glm::vec3(0, 90.3, 0), glm::vec3(0, 90, -1), glm::float32(60), glm::float32(windowWidth) / glm::float32(windowHeight));

	raytracer->reset();

	raytracer->enqueueModel({
		Tachyon::Rendering::GeometryPrimitive(glm::vec3(0, 90, -1), 0.5),
		Tachyon::Rendering::GeometryPrimitive(glm::vec3(0.75, 90, -1.5), 0.25),
		Tachyon::Rendering::GeometryPrimitive(glm::vec3(0, -10.5, -1), 100),
		}, Tachyon::Rendering::RenderingPipeline::GeometryInsertAttributes(glm::mat4(1), 0));

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Get window size and reset aspect
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		camera.setAspect(windowWidth, windowHeight);

		// Render the scene
		raytracer->render(Tachyon::Core::HDR(), camera);

		glfwSwapBuffers(window);
	}

	// Destroy the renderer
	raytracer.reset();

	// Destroy the renderer surface
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

    return EXIT_SUCCESS;
}
