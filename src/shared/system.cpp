#include "system.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

bool CreateChildProcess(char **argv, int argc, ProcessHandle *out_process_handle)
{
    bool result = false;
    #if defined(_WIN32)
        char cmd[256];
        size_t cmd_length = 0;
        int win32_argc = argc - 1; // on win32 we ignore the null argument
        for(int arg = 0; arg < win32_argc; arg++)
        {
            size_t arg_length = strlen(argv[arg]);
            memcpy(cmd + cmd_length, argv[arg], arg_length);
            cmd_length += arg_length;
            if(arg < win32_argc - 1) cmd[cmd_length++] = ' ';
        }
        cmd[cmd_length + 1] = '\0';

        STARTUPINFOA startup_info;
        GetStartupInfoA(&startup_info);
        result = CreateProcessA(NULL, cmd, NULL, NULL, true, 0, NULL,
                                NULL, &startup_info, &out_process_handle->info);
    #else
        pid_t pid = fork();
        if(pid)
            out_process_handle->pid = pid;
        else
            execv(argv[0], argv);
        result = true;
    #endif
    return result;
}

bool KillChildProcess(ProcessHandle *process_handle)
{
    bool result = false;
    #if defined(_WIN32)
        result = TerminateProcess(process_handle->info.hProcess, 0);
    #else
        result = kill(process_handle->pid, SIGKILL);
    #endif
    return result;
}

unsigned int GetExecutablePath(char *buffer, size_t buffer_length)
{
    unsigned int length = 0;
    #if defined(_WIN32)
        length = (unsigned int)GetModuleFileNameA(0, buffer, (DWORD)buffer_length);
    #else
        length = (unsigned int)readlink("/proc/self/exe", buffer, buffer_length);
    #endif
    return length;
}
