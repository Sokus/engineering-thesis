if(DISTRIBUTE)
	set(RESOURCE_PATH "./resources")
else()
	set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/resources")
endif()

if (MSVC)
    add_compile_options(/nologo /DRESOURCE_PATH="${RESOURCE_PATH}")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-D RESOURCE_PATH="${RESOURCE_PATH}")
endif()