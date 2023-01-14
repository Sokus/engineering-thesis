#include "buffer.h"

namespace GL { 

    Buffer::Buffer() {
        glGenBuffers(1, &handle);
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &handle);
    }

    void Buffer::Bind(GLenum target) {
        glBindBuffer(handle, target);
    }
}