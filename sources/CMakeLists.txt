if (USE_VULKAN)
	message("info: Vulkan renderer enabled")
	file(GLOB_RECURSE VULKAN_SOURCES Rendering/Vulkan/*.cpp)
elseif(USE_OPENGL)
	message("info: OpenGL renderer enabled, disabling the renderer")
	file(GLOB_RECURSE OPENGL_SOURCES Rendering/OpenGL/*.cpp)
endif()

add_executable(Tachyon main.cpp Core/Camera.cpp Core/Triangle.cpp Core/HDR.cpp Rendering/RenderingPipeline.cpp Rendering/GeometryPrimitive.cpp ${OPENGL_SOURCES} ${VULKAN_SOURCES})

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

set(GLSL_TARGET_SHADER_DIR "opengl")
set(GLSL_TARGET_ENV "opengl4.5")
set(GLSL_EMBED_TARGET_POSTFIX "OGL")
set(EMBEDDED_GLSL_SHADERS_DIR ${CMAKE_BINARY_DIR}/opengl)

set(SHADERS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sources/shaders)
if(USE_OPENGL)
	add_compile_definitions(OPENGL_SUPPORT)

	add_dependencies(Tachyon gl3w)
	target_link_libraries(Tachyon gl3w)
elseif(USE_VULKAN)
	add_compile_definitions(VULKAN_SUPPORT)

	include_directories(${Vulkan_INCLUDE_DIRS})

	target_link_libraries(Tachyon ${Vulkan_LIBRARIES})

	set(GLSL_TARGET_SHADER_DIR "vulkan")
	set(GLSL_TARGET_ENV "vulkan")
	set(GLSL_EMBED_TARGET_POSTFIX "VK")
	set(EMBEDDED_GLSL_SHADERS_DIR ${CMAKE_BINARY_DIR}/vulkan)
endif()

include_directories(${EMBEDDED_GLSL_SHADERS_DIR})

add_custom_target(
	spirv_shaders ALL
	
	COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GLSL_SHADERS_DIR}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GLSL_SHADERS_DIR}/shaders
	COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBEDDED_GLSL_SHADERS_DIR}/shaders/opengl

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -DRENDER -o "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_render.comp.spv" "${SHADERS_SOURCE_DIR}/programs/render.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_render.comp.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/raytrace_render.comp.spv.h" "raytrace_render_comp${GLSL_EMBED_TARGET_POSTFIX}"

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_FLUSH -o "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_flush.comp.spv" "${SHADERS_SOURCE_DIR}/programs/flush.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_flush.comp.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/raytrace_flush.comp.spv.h" "raytrace_flush_comp${GLSL_EMBED_TARGET_POSTFIX}"

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -DBVH_INSERT -o "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_insert.comp.spv" "${SHADERS_SOURCE_DIR}/programs/insert.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_insert.comp.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/raytrace_insert.comp.spv.h" "raytrace_insert_comp${GLSL_EMBED_TARGET_POSTFIX}"

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -DTLAS_UPDATE -o "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_update.comp.spv" "${SHADERS_SOURCE_DIR}/programs/update.comp"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/raytrace_update.comp.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/raytrace_update.comp.spv.h" "raytrace_update_comp${GLSL_EMBED_TARGET_POSTFIX}"

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GLSL_SHADERS_DIR}/tonemapping.vert.spv" "${SHADERS_SOURCE_DIR}/programs/tonemapping.vert"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/tonemapping.vert.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/tonemapping.vert.spv.h" "tonemapping_vert${GLSL_EMBED_TARGET_POSTFIX}"

	COMMAND glslc --target-env=${GLSL_TARGET_ENV} ${GLSL_COMPILE_DEBUG_FLAG} -o "${EMBEDDED_GLSL_SHADERS_DIR}/tonemapping.frag.spv" "${SHADERS_SOURCE_DIR}/programs/tonemapping.frag"
	COMMAND bin2c_serialize "${EMBEDDED_GLSL_SHADERS_DIR}/tonemapping.frag.spv" "${EMBEDDED_GLSL_SHADERS_DIR}/shaders/${GLSL_TARGET_SHADER_DIR}/tonemapping.frag.spv.h" "tonemapping_frag${GLSL_EMBED_TARGET_POSTFIX}"

	DEPENDS bin2c_serialize
		
	COMMENT "Compiling shaders to SPIR-V"
	SOURCES ${SHADERS_SOURCE_DIR}/programs/render.comp ${SHADERS_SOURCE_DIR}/programs/flush.comp ${SHADERS_SOURCE_DIR}/programs/insert.comp ${SHADERS_SOURCE_DIR}/programs/update.comp ${SHADERS_SOURCE_DIR}/programs/tonemapping.vert ${SHADERS_SOURCE_DIR}/programs/tonemapping.frag
	)

add_dependencies(Tachyon spirv_shaders)