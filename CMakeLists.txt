cmake_minimum_required (VERSION 3.10)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_DEBUG_POSTFIX "_d")

project (SparkEngine)

include(${PROJECT_SOURCE_DIR}/cmake/file2c.cmake)

# Set new policy when searching for external libs
cmake_policy(SET CMP0074 NEW)

# Multithreading (avoid problems when using std::thread and std::async)
find_package (Threads)

# Include GLM
include_directories(${PROJECT_SOURCE_DIR}/external/glm/glm)

# Include GL3W
add_subdirectory(${PROJECT_SOURCE_DIR}/external/gl3w)
include_directories(${PROJECT_SOURCE_DIR}/external/gl3w/include)

# Include GLFW
# OpenGL library new profile if possible
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)

find_package(glfw3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES( ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
LINK_DIRECTORIES( ${ARCHIVE_OUTPUT_DIRECTORY} )
set(PROJECTS_CONFIGURATION_FILES ${CMAKE_BINARY_DIR}/config)

# Include the main project sources
include_directories(${PROJECT_SOURCE_DIR}/sources)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources)
