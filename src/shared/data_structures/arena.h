#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h> // size_t

class Arena
{
private:
    void *buffer;
    size_t size;
    size_t used;

    bool free_on_destroy;
public:
    Arena();
    Arena(void *buffer, size_t size);
    Arena(size_t size);
    ~Arena();

    void Init(void *buffer, size_t size);
    void Init(size_t size);
    void Clear();
    bool WouldOverflow(size_t bytes);

    void *Push(size_t size);
    void Pop(size_t size);

    size_t BytesUsed();
    void Rewind(size_t offset);
};

#endif // DATA_STRUCTURES_H