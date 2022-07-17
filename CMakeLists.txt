cmake_minimum_required (VERSION 3.5)

project(praca_inzynierska)

if(UNIX)
	set(PI_TARGET_UNIX 1)
elseif(WIN32)
	set(PI_TARGET_WINDOWS 1)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})


add_executable(${PROJECT_NAME} src/win32_main.cpp)
add_subdirectory(ext)

target_link_libraries(${PROJECT_NAME}
	SDL2::SDL2
	glad::glad
	imgui::imgui
	stb::image
)