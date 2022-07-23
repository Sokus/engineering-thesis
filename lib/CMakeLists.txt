cmake_minimum_required (VERSION 3.5)

project(ext)

add_library(glad glad/src/glad.c)
add_library(glad::glad ALIAS glad)
target_include_directories(glad
	PUBLIC glad/include)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
find_package(SDL2 REQUIRED)

if(SDL_SHARED)
install(TARGETS SDL2
		DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

add_subdirectory(glm)

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
target_link_libraries(imgui PRIVATE SDL2::SDL2)

add_library(stb INTERFACE)
add_library(stb::image ALIAS stb)
target_include_directories(stb
	INTERFACE ${PROJECT_SOURCE_DIR})
