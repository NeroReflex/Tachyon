if (USE_VULKAN)
	message("info: Vulkan renderer enabled")
	file(GLOB_RECURSE VULKAN_SOURCES Rendering/Vulkan/*.cpp)
elseif(USE_OPENGL)
	message("info: OpenGL renderer enabled, disabling the renderer")
	file(GLOB_RECURSE OPENGL_SOURCES Rendering/OpenGL/*.cpp)
endif()

add_executable(Tachyon main.cpp Core/Camera.cpp Core/HDR.cpp Rendering/RenderingPipeline.cpp Rendering/GeometryPrimitive.cpp ${OPENGL_SOURCES} ${VULKAN_SOURCES})

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
if(USE_OPENGL)
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

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DRENDER -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_render.comp.spv" "${SHADERS_SOURCE_DIR}/opengl/render.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_render.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_render.comp.spv.h" "raytrace_render_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_FLUSH -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${SHADERS_SOURCE_DIR}/opengl/flush.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_flush.comp.spv.h" "raytrace_flush_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DBVH_INSERT -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${SHADERS_SOURCE_DIR}/opengl/insert.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_insert.comp.spv.h" "raytrace_insert_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_UPDATE -o "${EMBEDDED_GL_SHADERS_DIR}/raytrace_update.comp.spv" "${SHADERS_SOURCE_DIR}/opengl/update.comp"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/raytrace_update.comp.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/raytrace_update.comp.spv.h" "raytrace_update_compOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${SHADERS_SOURCE_DIR}/opengl/tonemapping.vert"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.vert.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/tonemapping.vert.spv.h" "tonemapping_vertOGL"

		COMMAND glslc --target-env=opengl4.5 ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${SHADERS_SOURCE_DIR}/opengl/tonemapping.frag"
		COMMAND bin2c_serialize "${EMBEDDED_GL_SHADERS_DIR}/tonemapping.frag.spv" "${EMBEDDED_GL_SHADERS_DIR}/shaders/opengl/tonemapping.frag.spv.h" "tonemapping_fragOGL"

		DEPENDS bin2c_serialize
		
		COMMENT "Compiling OpenGL shaders to SPIR-V"
		SOURCES ${SHADERS_SOURCE_DIR}/opengl/render.comp ${SHADERS_SOURCE_DIR}/opengl/flush.comp ${SHADERS_SOURCE_DIR}/opengl/insert.comp ${SHADERS_SOURCE_DIR}/opengl/update.comp ${SHADERS_SOURCE_DIR}/opengl/tonemapping.vert ${SHADERS_SOURCE_DIR}/opengl/tonemapping.frag
	)

	add_dependencies(Tachyon spirv_opengl_shaders)
elseif(USE_VULKAN)
	add_compile_definitions(VULKAN_SUPPORT)

	include_directories(${Vulkan_INCLUDE_DIRS})

	target_link_libraries(Tachyon ${Vulkan_LIBRARIES})

	# Generate OpenGL SPIR-V
	set(EMBEDDED_VULKAN_SHADERS_DIR ${CMAKE_BINARY_DIR}/vulkan)
	include_directories(${EMBEDDED_VULKAN_SHADERS_DIR})

	add_custom_target(
		spirv_vulkan_shaders ALL
	
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_VULKAN_SHADERS_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_VULKAN_SHADERS_DIR}/shaders
		COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/vulkan

		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -DRENDER -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_render.comp.spv" "${SHADERS_SOURCE_DIR}/vulkan/render.comp"
		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_render.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/vulkan/raytrace_render.comp.spv.h" "raytrace_render_compVK"

		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_FLUSH -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_flush.comp.spv" "${SHADERS_SOURCE_DIR}/vulkan/flush.comp"
		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/vulkan/raytrace_flush.comp.spv.h" "raytrace_flush_compVK"

		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -DBVH_INSERT -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_insert.comp.spv" "${SHADERS_SOURCE_DIR}/vulkan/insert.comp"
		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/vulkan/raytrace_insert.comp.spv.h" "raytrace_insert_compVK"

		COMMAND glslc --target-env=vulkan ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_UPDATE -o "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_update.comp.spv" "${SHADERS_SOURCE_DIR}/vulkan/update.comp"
		COMMAND bin2c_serialize "${EMBEDDED_VULKAN_SHADERS_DIR}/raytrace_update.comp.spv" "${EMBEDDED_VULKAN_SHADERS_DIR}/shaders/vulkan/raytrace_update.comp.spv.h" "raytrace_update_compVK"

		DEPENDS bin2c_serialize
		
		COMMENT "Compiling Vulkan shaders to SPIR-V"
		SOURCES ${SHADERS_SOURCE_DIR}/vulkan/render.comp ${SHADERS_SOURCE_DIR}/vulkan/flush.comp ${SHADERS_SOURCE_DIR}/vulkan/insert.comp ${SHADERS_SOURCE_DIR}/vulkan/update.comp
	)

	add_dependencies(Tachyon spirv_vulkan_shaders)
endif()


