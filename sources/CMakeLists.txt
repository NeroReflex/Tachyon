file(GLOB_RECURSE TACHYON_SOURCES *.cpp)
add_executable(Tachyon ${TACHYON_SOURCES})

# Set C++11 standard
set_property(TARGET Tachyon PROPERTY CXX_STANDARD 11)

add_dependencies(Tachyon glfw)
add_dependencies(Tachyon gl3w)

target_link_libraries(Tachyon
	${CMAKE_THREAD_LIBS_INIT}
	glfw
	gl3w
)

# Generate SPIR-V
set(EMBEDDED_GL_SHADERS_DIR ${CMAKE_BINARY_DIR}/opengl/)
set(OPENGL_SHADERS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sources/shaders/OpenGL/)
include_directories(${EMBEDDED_GL_SHADERS_DIR})

add_custom_command(
	OUTPUT ${EMBEDDED_GL_SHADERS_DIR}/shaders/raytrace.comp.spv.h
	INPUT ${OPENGL_SHADERS_SOURCE_DIR}/raytrace.comp
	COMMAND glslangValidator -G -o "raytrace.comp.spv" "${OPENGL_SHADERS_SOURCE_DIR}/raytrace.comp"
	COMMAND bin2c_serialize "raytrace.comp.spv" "shaders/raytrace.comp.spv.h" "raytrace_compOGL"
	WORKING_DIRECTORY ${EMBEDDED_GL_SHADERS_DIR}
	COMMENT "Generate SPIR-V of OpenGL raytracer compute shader"
)

add_custom_command(
	OUTPUT ${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.vert.spv.h
	INPUT ${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.vert
	COMMAND glslangValidator -G -o "tonemapping.vert.spv" "${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.vert"
	COMMAND bin2c_serialize "tonemapping.vert.spv" "shaders/tonemapping.vert.spv.h" "tonemapping_vertOGL"
	WORKING_DIRECTORY ${EMBEDDED_GL_SHADERS_DIR}
	COMMENT "Generate SPIR-V of OpenGL raytracer compute shader"
)

add_custom_command(
	OUTPUT ${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.frag.spv.h
	INPUT ${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.frag
	COMMAND glslangValidator -G -o "tonemapping.frag.spv" "${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.frag"
	COMMAND bin2c_serialize "tonemapping.frag.spv" "shaders/tonemapping.frag.spv.h" "tonemapping_fragOGL"
	WORKING_DIRECTORY ${EMBEDDED_GL_SHADERS_DIR}
	COMMENT "Generate SPIR-V of OpenGL tonemapping fragment shader"
)

add_custom_target(spirv_shaders ALL
	DEPENDS bin2c_serialize
	DEPENDS ${EMBEDDED_GL_SHADERS_DIR}/shaders/raytrace.comp.spv.h
	DEPENDS ${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.vert.spv.h
	DEPENDS ${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.frag.spv.h
)

add_dependencies(Tachyon spirv_shaders)
