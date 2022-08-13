if (MSVC)
    add_compile_options(/nologo)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# MSVC COMPILER OPTIONS:
# /MT        Compiles to create a multithreaded executable file, by using LIBCMT.lib.
# /MTd       Compiles to create a debug multithreaded executable file, by using LIBCMTD.lib.
# /GR-       Enables run-time type information (RTTI).
# /EHa-      Enable C++ exception handling (with SEH exceptions).
# /O<d/1/2>  Set optimization level.
# /Oi        Generates intrinsic functions.
# /WX        Treat warnings as errors.
# /W<n>      Set output warning level.
# /wd<n> 	   Disable the specified warning.
# /FC        Displays the full path of source code files passed to cl.exe in diagnostic text.
# /Z7        Generates C 7.0-compatible debugging information.
# /Fe:<path> Specifies a name and a directory for the .exe file or DLL created by the compiler.

# WARNING CODES:
# C4201      nonstandard extension used : nameless struct/union
# C4100      'identifier' : unreferenced formal parameter
# C4189      'identifier' : local variable is initialized but not referenced
# C4505      'function' : unreferenced local function has been removed

# MSVC LINKER OPTIONS:
# /OPT:REF   Eliminates functions and data that are never referenced

# WarningFlags=/W4 /wd4201 /wd4100 /wd4189 /wd4505
# DebugFlags=/Od /Z7 /MTd
# SET DebugFlags=/O2 /MT
# CommonCompilerFlags=/FC /Oi /GR- /EHa- %WarningFlags% %DebugFlags%