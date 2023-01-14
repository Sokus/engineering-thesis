#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "gl.h"
#include <string>

namespace GL {

    struct ShaderProgram {
        
        private:
        GLuint handle;

        public:
        ShaderProgram(const char *vertexSource, const char *fragmentSource);
        ~ShaderProgram();
        ShaderProgram(const ShaderProgram &) = delete;

        void Bind();
    };
}

#endif