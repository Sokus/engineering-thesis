#include "opengl3_utils.h"

#include <stdio.h>
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "platform/filesystem.h"

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

glm::mat4 CreateProjectionMatrix(int screen_width, int screen_height, float scale)
{
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

    float half_screen_width = (float)screen_width / 2.0f;
    float half_screen_height = (float)screen_height / 2.0f;
    glm::mat4 orthographic_projection = glm::ortho(-half_screen_width, half_screen_width,
                                                -half_screen_height, half_screen_height,
                                                -1.0f, 1.0f);
    glm::mat4 result = orthographic_projection * scale_matrix;
    return result;
}

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
        fprintf(stderr, "ERROR: Failed to compile shader (type=%s): %s\n", StringifyGLenum(type), infoLog);
        free(infoLog);
    }
    return shader;
}

GLuint CreateProgram(const char *vertex_shader_source,
                     const char *fragment_shader_source)
{
    int success;
    char info_log[512];

    GLuint vertex_shader_handle = CreateShader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader_handle = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_source);

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
    GLuint result = CreateProgram(
        ReadFileText(vertex_shader_path).c_str(),
        ReadFileText(fragment_shader_path).c_str());
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