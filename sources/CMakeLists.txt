# The SeaDragon resources library
file(GLOB_RECURSE TACHYON_SOURCES *.cpp)
add_executable(Tachyon ${TACHYON_SOURCES})

# Set C++11 standard
set_property(TARGET Tachyon PROPERTY CXX_STANDARD 11)

target_link_libraries(Tachyon
	${CMAKE_THREAD_LIBS_INIT})