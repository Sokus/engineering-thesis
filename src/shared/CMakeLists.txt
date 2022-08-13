project(shared)

file(GLOB SHARED_SRC *.cpp)

add_library(shared STATIC
    ${SHARED_SRC}
)

target_include_directories(shared
    PRIVATE
        ${CMAKE_BINARY_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

if(PLATFORM_WINDOWS)
	target_link_libraries(shared
		Ws2_32.lib
	)
endif()