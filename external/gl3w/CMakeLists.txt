include_directories(include)

add_library(gl3w STATIC src/gl3w.c)

target_link_libraries(gl3w ${CMAKE_DL_LIBS})

# Use C++11 standard
set_property(TARGET gl3w PROPERTY CXX_STANDARD 11)