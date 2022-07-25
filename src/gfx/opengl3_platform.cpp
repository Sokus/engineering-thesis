#include "base/base.h"
#include "os/filesystem.h"

class LazyProgram
{
    GLuint _handle = 0;
    const char *vertex_shader_path, *fragment_shader_path;

    public:
    LazyProgram(const LazyProgram &) = delete;
    LazyProgram(const char *vertex_shader_path, const char *fragment_shader_path) :
        vertex_shader_path(vertex_shader_path),
        fragment_shader_path(fragment_shader_path)
    {}

    operator GLuint() {
        if(!_handle)
            _handle = CreateProgramFromFiles(vertex_shader_path, fragment_shader_path);
        return _handle;
    }
};

struct EntityProgram
{
    GLuint program_handle;
    GLuint vao_handle;
    GLuint vbo_handle;
};

EntityProgram CreateEntityProgram(const char *vertex_shader_source,
                                  const char *fragment_shader_source)
{
    GLuint vao_handle, vbo_handle;
    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);
    glBindVertexArray(vao_handle);

    float quad_vertices[] =
    {
        // Position, UVs
        0.0f, 0.0f, 0.0f, 0.0f, // lower left
        1.0f, 0.0f, 1.0f, 0.0f,  // rower right
        1.0f, 1.0f, 1.0f, 1.0f,   // upper right

        0.0f, 0.0f, 0.0f, 0.0f, // lower right
        1.0f, 1.0f, 1.0f, 1.0f,   // upper right
        0.0f, 1.0f, 0.0f, 1.0f   // upper left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint program_handle = CreateProgram(vertex_shader_source, fragment_shader_source);

    EntityProgram result = {0};
    result.vao_handle = vao_handle;
    result.vbo_handle = vbo_handle;
    result.program_handle = program_handle;
    return result;
}