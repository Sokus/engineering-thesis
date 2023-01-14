#include "vao.h"

namespace GL {

    VAO::VAO() {
        glGenVertexArrays(1, &handle);
    }

    VAO::~VAO() {
        glDeleteVertexArrays(1, &handle);
    }

    void VAO::Bind() {
        glBindVertexArray(handle);
    }
}
        