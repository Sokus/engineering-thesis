cmake_minimum_required (VERSION 3.5)

project(ext)

add_library(glad glad/src/glad.c)
add_library(glad::glad ALIAS glad)
target_include_directories(glad
	PUBLIC glad/include)

add_library(sdl2 INTERFACE)
add_library(sdl2::sdl2 ALIAS sdl2)
target_include_directories(sdl2
	INTERFACE SDL/include)

file(GLOB IMGUI_SRC imgui/imgui*.cpp)
add_library(imgui
	imgui/backends/imgui_impl_sdl.cpp
	imgui/backends/imgui_impl_opengl3.cpp
    ${IMGUI_SRC})
add_library(imgui::imgui ALIAS imgui)
target_include_directories(imgui
	PUBLIC
		imgui
		imgui/backends)
target_link_libraries(imgui sdl2::sdl2)

add_library(stb INTERFACE)
add_library(stb::image ALIAS stb)
target_include_directories(stb
	INTERFACE ${PROJECT_SOURCE_DIR})
