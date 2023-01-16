#include "client.h"
#include "system.h"

extern AppData app_state;

void ShutdownServer()
{
    KillChildProcess(app_state.server_handle);
    app_state.server_handle = nullptr;
}

void LaunchServer(unsigned int server_port, Game::LevelType level_type)
{
    if (app_state.server_handle)
    {
        ShutdownServer();
    }

    char server_executable[255];
    char executable_path[255];
    unsigned int executable_path_length = GetExecutablePath(executable_path, sizeof(executable_path));
    unsigned int offset_to_one_past_last_slash = 0;
    for (unsigned int offset = 0; offset < executable_path_length; offset++)
    {
        if (executable_path[offset] == '\0')
            break;
        if (executable_path[offset] == '\\' || executable_path[offset] == '/')
            offset_to_one_past_last_slash = offset + 1;
    }
    memcpy(server_executable, executable_path, offset_to_one_past_last_slash);
#if defined(_WIN32)
    char server_executable_name[] = "server.exe";
#else
    char server_executable_name[] = "server";
#endif
    memcpy(server_executable + offset_to_one_past_last_slash,
           server_executable_name, sizeof(server_executable_name));

    char port_stringified[32];
    snprintf(port_stringified, sizeof(port_stringified), "%u", server_port);

    char level_stringified[32];
    snprintf(level_stringified, sizeof(level_stringified), "%d", level_type);

    char *launch_arguments[] = {
        server_executable,
        (char *)"-p",
        port_stringified,
        (char *)"-l",
        level_stringified,
        NULL};

    printf("launching: %s %s %s\n", server_executable, "-p", port_stringified);
    app_state.server_handle = CreateChildProcess(launch_arguments, ARRAY_SIZE(launch_arguments));
}