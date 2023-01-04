project(shared)

file(GLOB SHARED_SRC *.cpp)
file(GLOB SHARED_SERIALIZATION_SRC serialization/*.cpp)
file(GLOB SHARED_DATA_STRUCTURES_SRC data_structures/*.cpp)
file(GLOB SHARED_GAME_SRC game/*.cpp)
file(GLOB SHARED_LEVEL_SRC game/level/*.cpp)
file(GLOB SHARED_MATH_SRC math/*.cpp )

add_library(shared STATIC
    ${SHARED_SRC}
    ${SHARED_SERIALIZATION_SRC}
    ${SHARED_DATA_STRUCTURES_SRC}
    ${SHARED_GAME_SRC}
    ${SHARED_LEVEL_SRC}
    ${SHARED_MATH_SRC}
 "game/world.cpp"
 "game/level/level.cpp" "game/level/plains.cpp")

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