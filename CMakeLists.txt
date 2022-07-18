cmake_minimum_required (VERSION 3.5)
project(praca_inzynierska VERSION 0.1.0)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# This is Temporary
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

option(USE_INSTALL_RESOURCE_PATH "Set resource path to install location" OFF)

if(USE_INSTALL_RESOURCE_PATH)
	set(RESOURCE_PATH "./res")
else()
	set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/res")
endif()

configure_file(src/config.h.in config.h)

add_executable(${PROJECT_NAME} 	
	src/win32_main.cpp)

add_subdirectory(ext)

target_link_libraries(${PROJECT_NAME}
	SDL2::SDL2
	glad::glad
	imgui::imgui
	stb::image
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_BINARY_DIR}"
)

install(TARGETS ${PROJECT_NAME}
		DESTINATION "${CMAKE_INSTALL_PREFIX}")

install(DIRECTORY res
	DESTINATION "${CMAKE_INSTALL_PREFIX}")