#ifndef SYSTEM_H
#define SYSTEM_H

struct ProcessHandle;
ProcessHandle *CreateChildProcess(char **argv, int argc);
bool KillChildProcess(ProcessHandle *handle);

unsigned int GetExecutablePath(char *buffer, size_t buffer_length);

#endif // SYSTEM_H