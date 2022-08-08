cmake_minimum_required (VERSION 3.5)
project(praca_inzynierska)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(lib)

option(DISTRIBUTE "Compile for distribution" OFF)
option(ENABLE_LOG "Enable logging" ON)

if(DISTRIBUTE)
	set(RESOURCE_PATH "./resources")
else()
	set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/resources")
endif()

if(WIN32)
	set(PLATFORM_WINDOWS ON)
else()
	set(PLATFORM_WINDOWS OFF)
endif()

if(UNIX)
	set(PLATFORM_LINUX ON)
else()
	set(PLATFORM_LINUX OFF)
endif()

configure_file(cmake/config.h.in config.h)

if (MSVC)
    # target_compile_options(${PROJECT_NAME} PRIVATE /W4 -D _CRT_SECURE_NO_WARNINGS)
else()
    # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

file(GLOB BASE_SRC src/base/base_*.cpp)
file(GLOB SDL2_SRC src/platform/sdl2/sdl2_*.cpp)
file(GLOB OS_SRC src/platform/*.cpp)
file(GLOB GFX_SRC src/graphics/opengl3_*.cpp)

add_executable(${PROJECT_NAME}
	src/main.cpp
	${BASE_SRC}
	${SDL2_SRC}
	${OS_SRC}
	${GFX_SRC}
)

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

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
install(TARGETS ${PROJECT_NAME}
		DESTINATION "${CMAKE_INSTALL_PREFIX}")

install(DIRECTORY resources
	DESTINATION "${CMAKE_INSTALL_PREFIX}")
