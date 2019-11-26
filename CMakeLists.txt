cmake_minimum_required (VERSION 3.10)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_DEBUG_POSTFIX "_d")

project (Tachyon)

option(USE_OPENGL "Enable OpenGL support if the SKD is present" OFF)
option(USE_VULKAN "Enable vulkan support if the SDK is present" ON)

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

message("Using compiler: ${CMAKE_CXX_COMPILER_ID}")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Disable C++ exceptions.
  string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c-")
  add_definitions(-D_HAS_EXCEPTIONS=0)

  # Disable RTTI.
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
  
  # Enable C++20
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  /std:c++latest")
else(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Enable strict prototype warnings for C code in clang and gcc.
  if(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
  endif(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")

  # Disable C++ exceptions.
  string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Disable RTTI.
  string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
  
  # Enable C++20
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  -std=c++2a")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

# Include the main project sources
include_directories(${PROJECT_SOURCE_DIR}/sources)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources)
