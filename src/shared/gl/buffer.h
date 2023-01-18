#ifndef GL_BUFFER_H
#define GL_BUFFER_H

#include "gl.h"

namespace GL {

    struct Buffer {
        private:
        GLuint handle;

        public:
        Buffer();
        ~Buffer();
        Buffer(const Buffer &) = delete;

        void Bind(GLenum target = GL_ARRAY_BUFFER);
        template<typename T> void Upload(const T* data, int count, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STREAM_DRAW) {
            Bind(target);
            glBufferData(target, static_cast<GLsizei>(count)*sizeof(T), data, usage);
        }
    };
}

#endif