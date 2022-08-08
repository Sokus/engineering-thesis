@echo off

REM MSVC COMPILER OPTIONS:
REM /MT        Compiles to create a multithreaded executable file, by using LIBCMT.lib.
REM /MTd       Compiles to create a debug multithreaded executable file, by using LIBCMTD.lib.
REM /GR-       Enables run-time type information (RTTI).
REM /EHa-      Enable C++ exception handling (with SEH exceptions).
REM /O<d/1/2>  Set optimization level.
REM /Oi        Generates intrinsic functions.
REM /WX        Treat warnings as errors.
REM /W<n>      Set output warning level.
REM /wd<n> 	   Disable the specified warning.
REM /FC        Displays the full path of source code files passed to cl.exe in diagnostic text.
REM /Z7        Generates C 7.0-compatible debugging information.
REM /Fe:<path> Specifies a name and a directory for the .exe file or DLL created by the compiler.

REM WARNING CODES:
REM C4201      nonstandard extension used : nameless struct/union
REM C4100      'identifier' : unreferenced formal parameter
REM C4189      'identifier' : local variable is initialized but not referenced
REM C4505      'function' : unreferenced local function has been removed

REM MSVC LINKER OPTIONS:
REM /OPT:REF   Eliminates functions and data that are never referenced

SET WarningFlags=/W4 /wd4201 /wd4100 /wd4189 /wd4505
SET DebugFlags=/Od /Z7 /MTd
REM SET DebugFlags=/O2 /MT
SET CommonCompilerFlags=/nologo /FC /Oi /GR- /EHa- %WarningFlags% %DebugFlags%

SET CommonLinkerFlags=/SUBSYSTEM:CONSOLE /OPT:REF /LIBPATH:.\ext\SDL2\lib\x86 SDL2.lib SDL2main.lib user32.lib gdi32.lib winmm.lib Shell32.lib opengl32.lib
