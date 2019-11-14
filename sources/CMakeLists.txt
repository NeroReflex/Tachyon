set(OPENGL_SOURCES "")
set(OPENGL_SUPPORT false)
set(VULKAN_SOURCES "")
set(VULKAN_SUPPORT false)

if(OPENGL_FOUND)
	message("info: OpenGL renderer enabled, disabling the renderer")
	file(GLOB_RECURSE OPENGL_SOURCES Rendering/OpenGL/*.cpp)
	set(OPENGL_SUPPORT true)
else()
	message("Warning: OpenGL could not be found.")
endif()

if (VULKAN_FOUND)
	message("info: Vulkan renderer enabled")
	file(GLOB_RECURSE VULKAN_SOURCES Rendering/Vulkan/*.cpp)
	set(VULKAN_SUPPORT true)
else()
	message("Warning: Vulkan SDK could be found, disabling the renderer")
endif()

add_executable(Tachyon main.cpp Core/Camera.cpp Rendering/RenderingPipeline.cpp Rendering/GeometryPrimitive.cpp ${OPENGL_SOURCES} ${VULKAN_SOURCES})

# Set C++11 standard
set_property(TARGET Tachyon PROPERTY CXX_STANDARD 11)

add_dependencies(Tachyon glfw)

target_link_libraries(Tachyon
	${CMAKE_THREAD_LIBS_INIT}
	glfw
)

set(GLSL_COMPILE_DEBUG_FLAG "")
string(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Debug" is_debug_build)
if (is_debug_build)
	message("info: Compiling shaders for Debug")
	set(GLSL_COMPILE_DEBUG_FLAG "-g")
endif()

set(SHADERS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sources/shaders)

if(OPENGL_SUPPORT)
	add_compile_definitions(OPENGL_SUPPORT)

	add_dependencies(Tachyon gl3w)
	target_link_libraries(Tachyon gl3w)

	# Generate OpenGL SPIR-V
	set(EMBEDDED_GL_SHADERS_DIR ${CMAKE_BINARY_DIR}/opengl)
	include_directories(${EMBEDDED_GL_SHADERS_DIR})

	add_custom_target(
		spirv_opengl_shaders ALL
	
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GL_SHADERS_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GL_SHADERS_DIR}/shaders
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DRENDER -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_render.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_render.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_render.comp.spv.h" "raytrace_render_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_FLUSH -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_flush.comp.spv.h" "raytrace_flush_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DBVH_INSERT -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_insert.comp.spv.h" "raytrace_insert_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_UPDATE -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_update.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_update.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_update.comp.spv.h" "raytrace_update_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${SHADERS_SOURCE_DIR}/tonemapping.vert"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/tonemapping.vert.spv.h" "tonemapping_vertOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${SHADERS_SOURCE_DIR}/tonemapping.frag"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/tonemapping.frag.spv.h" "tonemapping_fragOGL"

		DEPENDS bin2c_serialize
		
		COMMENT "Compiling OpenGL shaders to SPIR-V"
		SOURCES ${SHADERS_SOURCE_DIR}/raytrace.comp ${SHADERS_SOURCE_DIR}/tonemapping.vert ${SHADERS_SOURCE_DIR}/tonemapping.frag
	)

	add_dependencies(Tachyon spirv_opengl_shaders)
endif(OPENGL_SUPPORT)

if(VULKAN_SUPPORT)
	add_compile_definitions(VULKAN_SUPPORT)

	include_directories(${Vulkan_INCLUDE_DIRS})

	target_link_libraries(Tachyon ${Vulkan_LIBRARIES})

	# Generate OpenGL SPIR-V
	set(EMBEDDED_VULKAN_SHADERS_DIR ${CMAKE_BINARY_DIR}/vulkan)
	include_directories(${EMBEDDED_GL_SHADERS_DIR})

#	add_custom_target(
#		spirv_vulkan_shaders ALL
#	
#		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_VULKAN_SHADERS_DIR}
#		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_VULKAN_SHADERS_DIR}/shaders
#
#		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -DRENDER -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_render.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_render.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/raytrace_render.comp.spv.h" "raytrace_render_compOGL"
#
#		COMMAND glslc --target-env=vulkan  ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_FLUSH -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_flush.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/raytrace_flush.comp.spv.h" "raytrace_flush_compOGL"
#
#		COMMAND glslc --target-env=vulkan  ${GLSL_COMPILE_DEBUG_FLAG} -DBVH_INSERT -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_insert.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/raytrace_insert.comp.spv.h" "raytrace_insert_compOGL"
#
#		COMMAND glslc --target-env=vulkan  ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_UPDATE -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_update.comp.spv" "${SHADERS_SOURCE_DIR}/raytrace.comp"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_update.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/raytrace_update.comp.spv.h" "raytrace_update_compOGL"
#
#		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_VULKAN_SHADERS_DIR}/tonemapping.vert.spv" "${SHADERS_SOURCE_DIR}/tonemapping.vert"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/tonemapping.vert.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/tonemapping.vert.spv.h" "tonemapping_vertOGL"
#
#		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_VULKAN_SHADERS_DIR}/tonemapping.frag.spv" "${SHADERS_SOURCE_DIR}/tonemapping.frag"
#		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/tonemapping.frag.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/tonemapping.frag.spv.h" "tonemapping_fragOGL"
#
#		DEPENDS bin2c_serialize
#		
#		COMMENT "Compiling OpenGL shaders to SPIR-V"
#		SOURCES ${SHADERS_SOURCE_DIR}/raytrace.comp ${SHADERS_SOURCE_DIR}/tonemapping.vert ${SHADERS_SOURCE_DIR}/tonemapping.frag
#	)
#
#	add_dependencies(Tachyon spirv_vulkan_shaders)
endif(VULKAN_SUPPORT)


