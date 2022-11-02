#include "glad/glad.h"

#include "opengl3_specialised_renderer.h"
#include "macros.h"

void SpecialisedRenderer::init(GLsizei _vertex_size)
{
    vertex_size = _vertex_size;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vbo_capacity = 128;
    glBufferData(GL_ARRAY_BUFFER, vbo_capacity * vertex_size, nullptr, GL_STREAM_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void SpecialisedRenderer::ensure_capacity(GLsizei no_vertices)
{
    // Reallocate the VBO if its too small
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (vbo_capacity < no_vertices)
    {
        vbo_capacity = MAX(no_vertices, vbo_capacity * 3 / 2);
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity * vertex_size, nullptr, GL_STREAM_DRAW);
    }
}

void SpecialisedRenderer::dispose()
{
    glDeleteProgram(shader_program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}