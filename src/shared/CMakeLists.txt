project(shared)

file(GLOB_RECURSE SHARED_CPP_SRC *.cpp)
file(GLOB_RECURSE SHARED_C_SRC *.c)

add_library(shared STATIC
    ${SHARED_CPP_SRC}
    ${SHARED_C_SRC}
)

target_include_directories(shared
    PRIVATE
        ${CMAKE_BINARY_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(shared
    raylib
)

if(WIN32)
    target_link_libraries(shared
        Shlwapi.lib # PathRemoveFileSpec
        Winmm.lib   # time
        wsock32.lib
        Ws2_32.lib  # sockets
        Dnsapi.lib  # DNS
	)
endif()