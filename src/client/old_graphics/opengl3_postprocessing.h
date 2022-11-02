#ifndef OPENGL3_POSTPROCESSING_H
#define OPENGL3_POSTPROCESSING_H

#include <glad/glad.h>

#include "opengl3_lazy_program.h"

void bind_dummy_vao();

extern LazyProgram
    identity_program,
    convolution_1d_program,
    extract_bright_fragments_program;

GLuint alloc_texture(int width, int height, GLenum internal_format);

// All framebuffers (and their textures) used by the application
struct Framebuffers
{
    int fbo_width, fbo_height;

    // Light map
    GLuint light_fbo;
    GLuint light_map;

    // Main fbo for drawing in-game entities
    GLuint draw_fbo;
    GLuint draw_texture;

    // Fbo used for computing the bloom effect
    GLuint bloom_fbo;
    GLuint bloom_texture;

    // Buffer fbo (e.g. for ping pong blur)
    GLuint buf_fbo;
    GLuint buf_texture;

    void init(int width, int height);
    void dispose();
};

void normalize_kernel_1d(float *kernel, int length);
void init_gaussian_blur_kernel_1d(float *kernel, int length, float variance);
void init_box_blur_kernel_1d(float *kernel, int length);

void ping_pong_blur(
    float *kernel, int length, int no_passes,
    GLuint fbo, GLuint tex,
    GLuint buf_fbo, GLuint buf_tex
);

#endif