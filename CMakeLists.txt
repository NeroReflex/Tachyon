cmake_minimum_required (VERSION 3.10)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_DEBUG_POSTFIX "_d")

project (Tachyon)

option(USE_OPENGL "Enable OpenGL support if the SKD is present" ON)
option(USE_VULKAN "Enable vulkan support if the SDK is present" OFF)

include(${PROJECT_SOURCE_DIR}/cmake/file2c.cmake)

# add the bin2c_serialize target
add_subdirectory(${PROJECT_SOURCE_DIR}/external/bin2c)

# Set new policy when searching for external libs
cmake_policy(SET CMP0074 NEW)

# Multithreading (avoid problems when using std::thread and std::async)
find_package (Threads)

# Include GLM
include_directories(${PROJECT_SOURCE_DIR}/external/glm/glm)

if (((USE_OPENGL) AND (USE_VULKAN)) OR ((NOT USE_OPENGL) AND (NOT USE_VULKAN)))
	message(FATAL_ERROR "O usi Vulkan o usi OpenGL. Deciditi, Porco Dio!")
endif()

if (USE_OPENGL)
	set(OpenGL_GL_PREFERENCE GLVND) # OpenGL library new profile if possible
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_FOUND false)
endif()

if (USE_VULKAN)
	find_package(Vulkan REQUIRED)
else()
	set(VULKAN_FOUND false)
endif()

# Include GL3W
if (USE_OPENGL)
	if (OPENGL_FOUND)
		message("OpenGL found: including gl3w")
		add_subdirectory(${PROJECT_SOURCE_DIR}/external/gl3w)
		include_directories(${PROJECT_SOURCE_DIR}/external/gl3w/include)
	endif()
endif()

# Include GLFW
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
