project(server)

add_executable(server
    server_main.cpp
)

target_include_directories(server
    PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(server
    shared
)

install(TARGETS server DESTINATION "${CMAKE_INSTALL_PREFIX}")