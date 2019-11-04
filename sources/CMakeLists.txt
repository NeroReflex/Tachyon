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
set(EMBEDDED_GL_SHADERS_DIR ${CMAKE_BINARY_DIR}/opengl)
set(OPENGL_SHADERS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sources/shaders/OpenGL)
include_directories(${EMBEDDED_GL_SHADERS_DIR})

add_custom_target(
	spirv_shaders ALL
	
	COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GL_SHADERS_DIR}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GL_SHADERS_DIR}/shaders

	COMMAND glslangValidator -G -DTLAS_FLUSH -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${OPENGL_SHADERS_SOURCE_DIR}/raytrace.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/raytrace_flush.comp.spv.h" "raytrace_flush_compOGL"

	COMMAND glslangValidator -G -DBVH_INSERT -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${OPENGL_SHADERS_SOURCE_DIR}/raytrace.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/raytrace_insert.comp.spv.h" "raytrace_insert_compOGL"

	COMMAND glslangValidator -G -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.vert"
	COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.vert.spv.h" "tonemapping_vertOGL"

	COMMAND glslangValidator -G -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.frag"
	COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/tonemapping.frag.spv.h" "tonemapping_fragOGL"

	DEPENDS bin2c_serialize
	#WORKING_DIRECTORY ${EMBEDDED_GL_SHADERS_DIR}
	COMMENT "Compiling OpenGL shaders to SPIR-V"
	SOURCES ${OPENGL_SHADERS_SOURCE_DIR}/raytrace.comp ${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.vert ${OPENGL_SHADERS_SOURCE_DIR}/tonemapping.frag
)

add_dependencies(Tachyon spirv_shaders)
