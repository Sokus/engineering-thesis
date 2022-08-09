file(GLOB BASE_SRC base/base_*.cpp)
file(GLOB SDL2_SRC platform/sdl2/sdl2_*.cpp)
file(GLOB OS_SRC platform/*.cpp)
file(GLOB GFX_SRC graphics/opengl3_*.cpp)

add_executable(${CLIENT_NAME}
	main.cpp
	${BASE_SRC}
	${SDL2_SRC}
	${OS_SRC}
	${GFX_SRC}
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
        "${PROJECT_SOURCE_DIR}/src"
)