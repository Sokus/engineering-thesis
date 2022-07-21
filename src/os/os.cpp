#if OS_WINDOWS
# include "win32/win32_os.cpp"
#elif OS_LINUX
# error no linux platform implementation
#else
# error no platform implementation
#endif