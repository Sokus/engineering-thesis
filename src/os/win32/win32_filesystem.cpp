#include "../filesystem.h"

#include <stdio.h>
#include <windows.h>

#include "base/base.h"

void FreeFileMemory(EntireFile *file)
{
    if(file->data)
        VirtualFree(file->data, 0, MEM_RELEASE);
    file->data = 0;
    file->size = 0;
}

EntireFile ReadEntireFile(const char *filename, bool null_terminate)
{
    EntireFile result = {0};

    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,
                                     0, OPEN_EXISTING, 0, 0);

    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            unsigned int size_to_read = SafeTruncateU64(file_size.QuadPart);
            unsigned int total_size = size_to_read;
            if(null_terminate) total_size++;

            void *data = VirtualAlloc(0, total_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(data)
            {
                DWORD bytes_read;
                if(ReadFile(file_handle, data, size_to_read, &bytes_read, 0) && (size_to_read == bytes_read))
                {
                    result.data = data;
                    result.size = total_size;
                }
                else
                {
                    VirtualFree(data, 0, MEM_RELEASE);
                }
            }
            else
            {
                fprintf(stderr, "ERROR: Could not allocate memory for the file %s.\n", filename);
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Could not get the size of the file %s.\n", filename);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    }

    return result;
}