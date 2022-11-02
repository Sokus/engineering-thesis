#ifndef OPENGL3_SPECIALISED_RENDERER_H
#define OPENGL3_SPECIALISED_RENDERER_H

#include "glad/glad.h"

class SpecialisedRenderer
{
    private:
    GLsizei vertex_size;

    protected:
    GLuint vbo, vao, shader_program;
    GLsizei vbo_capacity;

    void init(GLsizei _vertex_size);
    void ensure_capacity(GLsizei no_vertices);

    public:
    void dispose();
};

#endif // OPENGL3_SPECIALISED_RENDERER_H