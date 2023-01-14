#include "renderer.h"

#include <gl/gl.h>
#include <stdio.h>

namespace Game {

    void DrawQueue::DrawLight(const Light &light) {
        lights.push_back(light);
    }

    void DrawQueue::Clear() {
        lights.clear();
    }



    Renderer::Renderer() :
        gbuffer({{GL_COLOR_ATTACHMENT0, GL_RGBA8}})
    {
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::ResizeFramebuffers(const glm::ivec2 &size) {
        gbuffer.Resize(size);
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::BeginGeometry() {
        gbuffer.BindForDrawing();
    }
    
    void Renderer::EndGeometry() {
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::Draw(const DrawQueue &dq) {
        gbuffer.CopyTo(GL::Framebuffer::Default);
        GL::Framebuffer::Default.BindForDrawing();
    }
}