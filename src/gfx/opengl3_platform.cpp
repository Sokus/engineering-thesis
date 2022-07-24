#include "base/base.h"
#include "os/filesystem.h"

GLuint CreateShader(GLenum type, const char *source)
{
    int compileStatus, infoLogLength;

    // Attempt to compile shader
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if(compileStatus != GL_TRUE)
    {
        // Fetch info log if compilation failed
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *infoLog = (char *)malloc(infoLogLength);
        glGetShaderInfoLog(shader, infoLogLength, &infoLogLength, infoLog);
        fprintf(stderr, "ERROR: Failed to compile shader (type=%s): %s\n", GLenumToCStr(type), infoLog);
        free(infoLog);
    }
    return shader;
}

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

class SpecialisedRenderer
{
    private:
    GLsizei vertex_size;

    protected:
    GLuint vbo, vao, shader_program;
    GLsizei vbo_capacity;

    void init(GLsizei _vertex_size)
    {
        vertex_size = _vertex_size;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        vbo_capacity = 128;
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity * vertex_size, nullptr, GL_STREAM_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void ensure_capacity(GLsizei no_vertices)
    {
        // Reallocate the VBO if its too small
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if(vbo_capacity < no_vertices)
        {
            vbo_capacity = MAX(no_vertices, vbo_capacity * 3/2);
            glBufferData(GL_ARRAY_BUFFER, vbo_capacity*vertex_size, nullptr, GL_STREAM_DRAW);
        }
    }

    public:

    void dispose()
    {
        glDeleteProgram(shader_program);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};