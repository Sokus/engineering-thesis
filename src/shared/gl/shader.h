#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <string>
#include "gl.h"
#include <gl/framebuffer.h>

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

        void SetUniform(const char *name, int value);
    };
}

#endif