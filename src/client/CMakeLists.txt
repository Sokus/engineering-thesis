project(client)

file(GLOB CLIENT_SRC *.cpp)
# file(GLOB CLIENT_GRAPHICS_SRC graphics/*.cpp)

add_executable(client
	${CLIENT_SRC}
	#${CLIENT_GRAPHICS_SRC}
)

target_link_libraries(client
	raylib
    shared
)

target_include_directories(client
	PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

install(TARGETS client DESTINATION "${CMAKE_INSTALL_PREFIX}")