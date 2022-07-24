#include "opengl3_utils.h"

/** Converts a GLenum to a C string.
 *
 *  Returns "???" for unknown (or unhandled) GLenums.
 */
const char *StringifyGLenum(GLenum value)
{
    #define STRINGIFY_CASE(enum_value) case enum_value: return #enum_value
    switch(value)
    {
        STRINGIFY_CASE(GL_VERTEX_SHADER);
        STRINGIFY_CASE(GL_FRAGMENT_SHADER);
        STRINGIFY_CASE(GL_GEOMETRY_SHADER);
        default: return "???";
    }
    #undef STRINGIFY_CASE
}

GLuint CreateProgram(const char *vertex_shader_source,
                     const char *fragment_shader_source)
{
    int success;
    char info_log[512];

    GLuint vertex_shader_handle   = CreateShader(GL_VERTEX_SHADER, vertex_shader_source),
           fragment_shader_handle = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_source);

    GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glLinkProgram(program_handle);
    glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(program_handle, 512, 0, info_log);
        fprintf(stderr, "ERROR: (program link) %s\n", info_log);
    }

    glDetachShader(program_handle, vertex_shader_handle);
    glDeleteShader(vertex_shader_handle);
    glDetachShader(program_handle, fragment_shader_handle);
    glDeleteShader(fragment_shader_handle);

    return program_handle;
}

GLuint CreateProgramFromFiles(const char *vertex_shader_path,
                              const char *fragment_shader_path)
{
    EntireFile vertex_shader_source = ReadEntireFile(vertex_shader_path, true);
    EntireFile fragment_shader_source = ReadEntireFile(fragment_shader_path, true);
    GLuint result = CreateProgram(
        (const char*)vertex_shader_source.data,
        (const char*)fragment_shader_source.data
    );
    FreeFileMemory(&vertex_shader_source);
    FreeFileMemory(&fragment_shader_source);
    return result;
}

void SetBoolUniform(GLuint program, const char *name, bool value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1i(uniform_location, (int)value);
}

void SetIntUniform(GLuint program, const char *name, int value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1i(uniform_location, value);
}

void SetFloatUniform(GLuint program, const char *name, float value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1f(uniform_location, value);
}

void SetVec2Uniform(GLuint program, const char *name, glm::vec2 value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform2f(uniform_location, value.x, value.y);
}

void SetMat4Uniform(GLuint program, const char *name, const glm::mat4& matrix)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void SetFloatArrayUniform(GLuint program, const char *name, const float *array, int count)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1fv(uniform_location, count, array);
}