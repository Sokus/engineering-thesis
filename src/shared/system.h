#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

struct ProcessHandle
{
    #if defined(_WIN32)
        PROCESS_INFORMATION info;
    #else
        pid_t pid;
    #endif
};

bool CreateChildProcess(char **argv, int argc, ProcessHandle *out_process_handle);
bool KillChildProcess(ProcessHandle *process_handle);

unsigned int GetExecutablePath(char *buffer, size_t buffer_length);

#endif // SYSTEM_H