#include <stdexcept>

#include "glad/glad.h"

void APIENTRY OpenGLDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void *user_pointer
) {
    if(type == GL_DEBUG_TYPE_ERROR)
        throw std::runtime_error(message);
}

void EnableThrowOnOpenGLErrors() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&OpenGLDebugCallback, 0);
}