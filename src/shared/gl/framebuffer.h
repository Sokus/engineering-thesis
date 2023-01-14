#ifndef GL_FRAMEBUFFER_H
#define GL_FRAMEBUFFER_H

#include <vector>
#include <map>
#include "gl.h"
#include <glm/vec2.hpp>

namespace GL {

    struct AttachmentSpec {
        GLenum attachment;
        GLenum internalFormat;
    };

    struct Framebuffer {

        private:
        GLuint handle;
        glm::ivec2 _size;
        std::vector<AttachmentSpec> specs;
        std::map<GLenum,GLuint> attachments;

        void Init();
        void Dispose();
        bool IsDefault() const;

        public:

        static Framebuffer Default;

        Framebuffer();
        Framebuffer(const std::vector<AttachmentSpec> &specs, const glm::ivec2 &size = {640,480});
        ~Framebuffer();
        Framebuffer(const Framebuffer &) = delete;

        void Bind(GLenum target);
        void BindForDrawing();

        glm::ivec2 Size() const;
        void Resize(const glm::ivec2 &newSize);
        void SetupViewport() const;

        GLuint GetTexture(GLenum attachment) const;

        void CopyTo(GL::Framebuffer &dst);
    };
}

#endif