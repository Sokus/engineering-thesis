#ifndef FILESYSTEM_H
#define FILESYSTEM_H

typedef struct EntireFile
{
    void *data;
    unsigned int size;
} EntireFile;

EntireFile ReadEntireFile(const char *filename, bool null_terminate);
void FreeFileMemory(EntireFile *file);

#endif //FILESYSTEM_H
