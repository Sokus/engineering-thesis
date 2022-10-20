project(shared)

file(GLOB SHARED_SRC *.cpp)
file(GLOB SHARED_NETWORKING_SRC networking/*.cpp)
file(GLOB SHARED_SERIALIZATION_SRC serialization/*.cpp)

add_library(shared STATIC
    ${SHARED_SRC}
    ${SHARED_NETWORKING_SRC}
    ${SHARED_SERIALIZATION_SRC}
)

target_include_directories(shared
    PRIVATE
        ${CMAKE_BINARY_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

if(PLATFORM_WINDOWS)
    target_link_libraries(shared
        Ws2_32.lib # sockets
        Dnsapi.lib # DNS
	)
endif()