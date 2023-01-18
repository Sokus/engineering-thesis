cmake_minimum_required (VERSION 3.5)
project(praca_inzynierska)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_CXX_STANDARD 17)

option(DISTRIBUTE "Compile for distribution" OFF)
option(ENABLE_LOG "Enable logging" ON)
set(CLIENT_NAME "client")
set(SERVER_NAME "server")

include(CompileFlags)

add_subdirectory(lib)
add_subdirectory(src/shared)
add_subdirectory(src/client)
add_subdirectory(src/server)

add_subdirectory(examples)

install(DIRECTORY resources
	DESTINATION "${CMAKE_INSTALL_PREFIX}")
