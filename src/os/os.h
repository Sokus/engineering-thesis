/* date = July 21st 2022 7:18 pm */

#ifndef OS_H
#define OS_H

typedef struct EntireFile
{
    void *data;
    unsigned int size;
} EntireFile;

EntireFile ReadEntireFile(const char *filename, bool null_terminate);
void FreeFileMemory(EntireFile *file);

#endif //OS_H
