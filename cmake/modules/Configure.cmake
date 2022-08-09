if(DISTRIBUTE)
	set(RESOURCE_PATH "./resources")
else()
	set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/resources")
endif()

if(WIN32)
	set(PLATFORM_WINDOWS ON)
else()
	set(PLATFORM_WINDOWS OFF)
endif()

if(UNIX)
	set(PLATFORM_LINUX ON)
else()
	set(PLATFORM_LINUX OFF)
endif()

configure_file(cmake/config.h.in config.h)