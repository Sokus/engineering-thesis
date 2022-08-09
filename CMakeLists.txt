cmake_minimum_required (VERSION 3.5)
project(praca_inzynierska)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(DISTRIBUTE "Compile for distribution" OFF)
option(ENABLE_LOG "Enable logging" ON)
set(CLIENT_NAME "client")
set(SERVER_NAME "server")
include(Configure)

add_subdirectory(lib)
add_subdirectory(src)
include(CompileFlags)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
install(TARGETS ${CLIENT_NAME}
		DESTINATION "${CMAKE_INSTALL_PREFIX}")

install(DIRECTORY resources
	DESTINATION "${CMAKE_INSTALL_PREFIX}")
