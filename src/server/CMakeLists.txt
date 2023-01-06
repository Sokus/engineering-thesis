project(server)

file(GLOB SERVER_SRC *.cpp)

add_executable(server
    ${SERVER_SRC}
)

target_include_directories(server
    PRIVATE
        "${CMAKE_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(server
    shared
    parg
)

install(TARGETS server DESTINATION "${CMAKE_INSTALL_PREFIX}")