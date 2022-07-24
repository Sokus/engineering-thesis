#include "opengl3_texture.h"

#include <stdio.h> // fprintf
#include <stdlib.h> // malloc, free

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h" // stbi_load, stbi_free

#include "base/base.h" // ASSERT

/**
 * Loads an image and stores it as a GL_TEXTURE_2D_ARRAY.
 */
void Texture::Load(char *path, int tile_width, int tile_height, int force_channels)
{
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, 0, force_channels);

    if(data)
    {
        int tiles_x = width / tile_width;
        int tiles_y = height / tile_height;
        int tile_count = tiles_x * tiles_y;
        ASSERT(tile_width > 0 && tile_height > 0 && tile_count > 0);

        if(width % tile_width != 0 || height % tile_height != 0)
        {
            fprintf(stderr,
                    "WARNING: Texture size not a "
                    "multiple of tile width/height! (%s)",
                    path);
        }

        GLint format = (force_channels == 3 ? GL_RGB :
                        force_channels == 4 ? GL_RGBA : 0);

        if(format == 0)
        {
            fprintf(stderr, "ERROR: Channels count (%d) not supported! (%s)", force_channels, path);
            stbi_image_free(data);
            return;
        }

        GLuint gl_texture_id;
        glGenTextures(1, &gl_texture_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, gl_texture_id);

        float texture_border_color[] = { 1.0f, 0.0f, 1.0f, 0.0f };
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, texture_border_color);

        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tile_width, tile_height,
                     tile_count, 0, (GLenum)format, GL_UNSIGNED_BYTE, 0);

        unsigned char *tile_buffer = (unsigned char *)malloc(tile_width * tile_height * force_channels);
        int tile_w_stride = force_channels * tile_width;
        int row_stride = tile_w_stride * tiles_x;
        for(int tile_y_idx = 0; tile_y_idx < tiles_y; ++tile_y_idx)
        {
            for(int tile_x_idx = 0; tile_x_idx < tiles_x; ++tile_x_idx)
            {
                int offset = tile_y_idx * row_stride * tile_height + tile_x_idx * tile_w_stride;
                unsigned char *tile_corner_ptr = data + offset;

                for(int tile_pixel_y = 0; tile_pixel_y < tile_height; ++tile_pixel_y)
                {
                    unsigned char *src = tile_corner_ptr + tile_pixel_y * row_stride;
                    int inverse_tile_pixeL_y = (tile_height - tile_pixel_y - 1);
                    unsigned char *dst = tile_buffer + inverse_tile_pixeL_y * tile_w_stride;
                    MEMORY_COPY(dst, src, (unsigned int)(tile_w_stride));
                }

                int layer_idx = tile_y_idx * tiles_x + tile_x_idx;

                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer_idx,
                                tile_width, tile_height, 1, (GLenum)format,
                                GL_UNSIGNED_BYTE, tile_buffer);
            }
        }
        free(tile_buffer);
        stbi_image_free(data);

        this->loaded = true;
        this->id = gl_texture_id;
        this->width = width;
        this->height = height;
        this->channels = force_channels;
        this->tile_width = tile_width;
        this->tile_height = tile_height;
        this->tile_count = tile_count;
    }
    else
    {
        fprintf(stderr, "ERROR: Could not load texture: %s\n", path);
    }
}

void Texture::Free()
{
    glDeleteTextures(1, &id);
    loaded = false;
}