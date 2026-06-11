# Set CMake minimum required version
cmake_minimum_required(VERSION 3.23)

project(gl-physics-engine)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ImGui source files
set(IMGUI_SOURCES
	dependencies/imgui/imgui.cpp
	dependencies/imgui/imgui_draw.cpp
	dependencies/imgui/imgui_widgets.cpp
	dependencies/imgui/imgui_tables.cpp
	dependencies/imgui/imgui_demo.cpp
	dependencies/imgui/backends/imgui_impl_glfw.cpp
	dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

#
add_executable(main)
file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
target_sources(main PRIVATE ${MAIN_SOURCES} ${IMGUI_SOURCES})

# Includes
target_include_directories(main PRIVATE
	src
	dependencies
	dependencies/glfw/include
	dependencies/imgui
	dependencies/imgui/backends
	dependencies/glm
)

# Link static libs
target_link_libraries(main PRIVATE
	glfw
)

# GLFW compilation
add_subdirectory(dependencies/GLFW)

# Automatically copy assets folder to the build/out directory
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
	# Hardcode - change later
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/Debug/assets"
)