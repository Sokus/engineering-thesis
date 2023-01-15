#include "renderer.h"

#include <gl/gl.h>
#include "programs.h"
#include <stdio.h>

namespace Game {

    void DrawQueue::DrawLight(const Light &light) {
        lights.push_back(light);
    }

    void DrawQueue::Clear() {
        lights.clear();
    }



    Renderer::Renderer() :
        gbuffer({{GL_COLOR_ATTACHMENT0, GL_RGBA8}}),
        hdrFbo({{GL_COLOR_ATTACHMENT0, GL_RGBA16F}})
    {
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::ResizeFramebuffers(const glm::ivec2 &size) {
        gbuffer.Resize(size);
        hdrFbo.Resize(size);
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::BeginGeometry() {
        gbuffer.BindForDrawing();
    }

    void Renderer::EndGeometry() {
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::Draw(const DrawQueue &dq) {

        glClearColor(0,0,0,0);
        glActiveTexture(GL_TEXTURE0);

        GL::Framebuffer::Default.BindForDrawing();
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, gbuffer.GetTexture(GL_COLOR_ATTACHMENT0));
        grayscaleProgram().SetUniform("tex", 0);
        grayscaleProgram().DrawPostprocessing();
    }
}