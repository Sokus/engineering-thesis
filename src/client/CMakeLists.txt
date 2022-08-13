project(client)

file(GLOB CLIENT_SRC *.cpp)
file(GLOB CLIENT_GRAPHICS_SRC graphics/*.cpp)

add_executable(client
	client_main.cpp
	${CLIENT_SRC}
	${CLIENT_GRAPHICS_SRC}
)

target_link_libraries(client
	SDL2::SDL2
	glad::glad
	glm::glm
	imgui::imgui
	stb::image
    shared
)

target_include_directories(client
	PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

install(TARGETS client DESTINATION "${CMAKE_INSTALL_PREFIX}")