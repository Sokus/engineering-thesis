#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <string>
#include "gl.h"
#include "texture.h"
#include <gl/framebuffer.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace GL {

    struct ShaderProgram {
        
        private:
        GLuint handle;

        public:
        ShaderProgram(const char *vertexSource, const char *fragmentSource);
        ~ShaderProgram();
        ShaderProgram(const ShaderProgram &) = delete;

        static ShaderProgram ForPostprocessing(const char *fragmentSource);

        void Bind();
        void DrawPostprocessing();

        ShaderProgram &SetUniform(const char *name, int value);
        ShaderProgram &SetUniform(const char *name, const TextureUnit &value);
        ShaderProgram &SetUniform(const char *name, float value);
        ShaderProgram &SetUniform(const char *name, const float *values, int count);
        ShaderProgram &SetUniform(const char *name, const glm::vec2 &value);
        ShaderProgram &SetUniform(const char *name, const glm::vec3 &value);
        ShaderProgram &SetUniform(const char *name, const glm::vec4 &value);
        ShaderProgram &SetUniform(const char *name, const glm::mat4 &value);
    };
}

#endif