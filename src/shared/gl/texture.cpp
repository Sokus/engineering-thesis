#include "texture.h"

namespace GL {

    TextureUnit::TextureUnit(unsigned int index) : index(index) {}

    void TextureUnit::Bind() {
        glActiveTexture(GL_TEXTURE0 + index);
    }

    void TextureUnit::AttachTexture2D(GLuint texture) {
        Bind();
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    
    unsigned int TextureUnit::Index() const {
        return index;
    }
}