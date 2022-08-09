#ifndef OPENGL3_TEXTURE_H
#define OPENGL3_TEXTURE_H

#include "glad/glad.h"

struct Texture
{
    bool loaded;

    GLuint id;
    int width;
    int height;
    int channels;
    int tile_width;
    int tile_height;
    int tile_count_x;
    int tile_count_y;
    int tile_count;

    void Load(const char *path, int tile_width, int tile_height, int force_channels);
    void Free();
};

#endif // OPENGL3_TEXTURE_H