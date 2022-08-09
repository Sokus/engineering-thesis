# Shared
file(GLOB SHARED_BASE_SRC shared/base/base*.cpp)
file(GLOB SHARED_SRC shared/*.cpp)

# Client
file(GLOB CLIENT_SDL2_SRC client/platform/sdl2/sdl2*.cpp)
file(GLOB CLIENT_PLATFORM_SRC client/platform/*.cpp)
file(GLOB CLIENT_GRAPHICS_SRC client/graphics/opengl3*.cpp)

add_executable(${CLIENT_NAME}
	client/client_main.cpp
	${SHARED_SRC}
    ${SHARED_BASE_SRC}
	${CLIENT_SDL2_SRC}
	${CLIENT_PLATFORM_SRC}
	${CLIENT_GRAPHICS_SRC}
)

target_link_libraries(${CLIENT_NAME}
	SDL2::SDL2
	glad::glad
	glm::glm
	imgui::imgui
	stb::image
)

target_include_directories(${CLIENT_NAME}
	PRIVATE
		"${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/src/client"
        "${PROJECT_SOURCE_DIR}/src/shared"
)

# Server
add_executable(${SERVER_NAME}
    server/server_main.cpp
	${SHARED_SRC}
    ${SHARED_BASE_SRC}
)

target_include_directories(${SERVER_NAME}
    PRIVATE
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/src/server"
        "${PROJECT_SOURCE_DIR}/src/shared"
)