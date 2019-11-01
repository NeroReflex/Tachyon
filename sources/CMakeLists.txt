file(GLOB_RECURSE TACHYON_SOURCES *.cpp)
add_executable(Tachyon ${TACHYON_SOURCES})

# Embed every shader into the library
set(EMBEDDED_GL_SHADERS_DIR ${CMAKE_BINARY_DIR}/opengl/shaders)
include_directories(${EMBEDDED_GL_SHADERS_DIR})
file(GLOB_RECURSE OPENGL_SHADERS_SOURCES ${PROJECT_SOURCE_DIR}/sources/shaders/OpenGL/*)
foreach(SHADER_SRC ${OPENGL_SHADERS_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME) # append  to have a working CMake
    string(TOUPPER "${SHADER_NAME}" SHADER_UPPERNAME)
    string(REPLACE "." "_" SHADER_C_NAME "${SHADER_UPPERNAME}")
    FILE2C("${SHADER_SRC}" "SHADER_${SHADER_C_NAME}" "${EMBEDDED_GL_SHADERS_DIR}/shaders/${SHADER_NAME}.h")
endforeach()

# Set C++11 standard
set_property(TARGET Tachyon PROPERTY CXX_STANDARD 11)

add_dependencies(Tachyon glfw)
add_dependencies(Tachyon gl3w)

target_link_libraries(Tachyon
	${CMAKE_THREAD_LIBS_INIT}
	glfw
	gl3w
)