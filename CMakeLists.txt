cmake_minimum_required (VERSION 3.5)

project(praca_inzynierska)

add_subdirectory(ext)

add_executable(${PROJECT_NAME} src/win32_main.cpp)

target_link_libraries(${PROJECT_NAME}
	sdl2::sdl2
	glad::glad
	imgui::imgui
	stb::image
)