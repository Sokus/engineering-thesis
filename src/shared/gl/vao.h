#ifndef GL_VAO_H
#define GL_VAO_H

#include "gl.h"

namespace GL {

    struct VAO {

        private:
        GLuint handle;

        public:
        
        VAO();
        ~VAO();
        VAO(const VAO &) = delete;

        void Bind();
    };
}

#endif