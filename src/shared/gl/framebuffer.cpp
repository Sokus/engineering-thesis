#include "framebuffer.h"
#include <stdio.h>

extern "C" {
    int GetRenderWidth(void);
    int GetRenderHeight(void);
}

namespace GL {

    Framebuffer Framebuffer::Default;

    bool Framebuffer::IsDefault() const {
        return handle == 0;
    }

    // Allocates an empty texture with specified dimensions and internal format
    GLuint AllocTexture2D(const glm::vec2 &size, GLenum internal_format) {
        GLuint texture_handle;
        glGenTextures(1, &texture_handle);
        glBindTexture(GL_TEXTURE_2D, texture_handle);
        glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, size.x, size.y);
        return texture_handle;
    }

    void Framebuffer::Init() {

        if(IsDefault()) return;

        glGenFramebuffers(1, &handle);
        Bind(GL_FRAMEBUFFER);

        for(auto &spec: specs) {
            GLuint texture = AllocTexture2D(_size, spec.internalFormat);
            glFramebufferTexture2D(GL_FRAMEBUFFER, spec.attachment, GL_TEXTURE_2D, texture, 0);
            attachments[spec.attachment] = texture;
        }
    }

    void Framebuffer::Dispose() {

        if(IsDefault()) return;

        glDeleteFramebuffers(1, &handle);

        for(const auto [_, texture] : attachments) {
            glDeleteTextures(1, &texture);
        }
        attachments.clear();
    }

    Framebuffer::Framebuffer() {
        handle = 0;
    }

    Framebuffer::Framebuffer(const std::vector<AttachmentSpec> &specs, const glm::ivec2 &size) : 
        _size(size),
        specs(specs)
    {
        handle = 999999;
        Init();
    }

    Framebuffer::~Framebuffer() {
        Dispose();
    }

    void Framebuffer::Bind(GLenum target) {
        glBindFramebuffer(target, handle);
    }

    void Framebuffer::BindForDrawing() {
        Bind(GL_DRAW_FRAMEBUFFER);
        SetupViewport();
    }

    // TODO this doesn't work for default framebuffer
    glm::ivec2 Framebuffer::Size() const {
        if(IsDefault()) return {GetRenderWidth(), GetRenderHeight()};
        else return _size;
    }

    void Framebuffer::Resize(const glm::ivec2 &newSize) {
        if(newSize == Size()) return;
        _size = newSize;
        Dispose();
        Init();
    }

    void Framebuffer::SetupViewport() const {
        glViewport(0, 0, Size().x, Size().y);
    }

    GLuint Framebuffer::GetTexture(GLenum attachment) const {
        auto it = attachments.find(attachment);
        if(it == attachments.end())
            return 999999;
        else
            return it->second;
    }

    void Framebuffer::CopyTo(GL::Framebuffer &dst) {
        Bind(GL_READ_FRAMEBUFFER);
        dst.Bind(GL_DRAW_FRAMEBUFFER);
        glBlitFramebuffer(0, 0, Size().x, Size().y, 0, 0, dst.Size().x, dst.Size().y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    void Framebuffer::PrintAttachmentInfo() {
        for(const auto [att, texture] : attachments) {
            printf("%d -> %d\n", att, texture);
        }
    }
}