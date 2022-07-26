cmake_minimum_required (VERSION 3.5)
project(praca_inzynierska VERSION 0.1.0)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# This is Temporary
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

option(DISTRIBUTE "Compile for distribution" OFF)

if(DISTRIBUTE)
	set(RESOURCE_PATH "./res")
else()
	set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/res")
endif()

configure_file(src/config.h.in config.h)

if(WIN32)
	file(GLOB WIN32_SRC src/os/win32/win32_*.cpp)
endif()

file(GLOB BASE_SRC src/base/base_*.cpp)
file(GLOB SDL2_SRC src/os/sdl2/sdl2_*.cpp)
file(GLOB GFX_SRC src/gfx/opengl3_*.cpp)

add_executable(${PROJECT_NAME}
	src/win32_main.cpp
	${BASE_SRC}
	${SDL2_SRC}
	${GFX_SRC}
	${WIN32_SRC}
)

add_subdirectory(lib)

target_link_libraries(${PROJECT_NAME}
	SDL2::SDL2
	glad::glad
	glm::glm
	imgui::imgui
	stb::image
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_BINARY_DIR}"
		src
)

install(TARGETS ${PROJECT_NAME}
		DESTINATION "${CMAKE_INSTALL_PREFIX}")

install(DIRECTORY res
	DESTINATION "${CMAKE_INSTALL_PREFIX}")