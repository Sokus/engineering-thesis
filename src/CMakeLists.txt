# Shared
file(GLOB SHARED_SRC shared/*.cpp)

# Client
file(GLOB CLIENT_SRC client/*.cpp)
file(GLOB CLIENT_GRAPHICS_SRC client/graphics/*.cpp)

add_executable(${CLIENT_NAME}
	client/client_main.cpp
	${SHARED_SRC}
	${CLIENT_SRC}
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