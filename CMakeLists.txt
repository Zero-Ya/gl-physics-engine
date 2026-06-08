# Set CMake minimum required version
cmake_minimum_required(VERSION 3.23)

project(gl-physics-engine)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
add_executable(main)
file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
target_sources(main PRIVATE ${MAIN_SOURCES})

# Includes
target_include_directories(main PRIVATE
	src
	dependencies
	dependencies/glfw/include
)

# Link static libs
target_link_libraries(main PRIVATE
	glfw
)

# GLFW compilation
add_subdirectory(dependencies/GLFW)