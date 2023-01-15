#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "gl.h"

namespace GL {
    struct TextureUnit {

        private:
        unsigned int index;

        public:
        TextureUnit(unsigned int index);
        void Bind();
        void AttachTexture2D(GLuint texture);
        unsigned int Index() const;
    };
}
#endif