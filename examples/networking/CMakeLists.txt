project(example_networking)

add_executable(example_client
	example_client.cpp
)

target_link_libraries(example_client
    shared
)

target_include_directories(example_client
	PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

install(TARGETS example_client DESTINATION "${CMAKE_INSTALL_PREFIX}")

add_executable(example_server
    example_server.cpp
)

target_include_directories(example_server
    PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(example_server
    shared
    parg
)

install(TARGETS example_server DESTINATION "${CMAKE_INSTALL_PREFIX}")