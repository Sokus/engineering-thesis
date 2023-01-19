#include "renderer.h"

#include <stdio.h>
#include <rlgl.h>
#include <raymath.h>
#include <gl/gl.h>
#include <gl/texture.h>
#include "programs.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Game {

    enum {
        ATT_GBUFFER_ALBEDO = GL_COLOR_ATTACHMENT0,
        ATT_GBUFFER_DEPTH = GL_COLOR_ATTACHMENT1
    };

    glm::mat4 ToGLMMatrix(const Matrix &m) {
        return {
            m.m0, m.m1, m.m2,  m.m3, 
            m.m4, m.m5, m.m6,  m.m7, 
            m.m8, m.m9, m.m10, m.m11, 
            m.m12, m.m13, m.m14, m.m15
        };
    }


    GL::TextureUnit
        TEX_ALBEDO_MAP(0),
        TEX_DEPTH_MAP(1),
        TEX_HDR(2),
        TEX_SDR1(3),
        TEX_SDR2(4);

    void BindTextureToTexUnit(GLuint texUnitIndex, GLuint texture) {
        glActiveTexture(GL_TEXTURE0 + texUnitIndex);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    Renderer::Renderer() :
        gbuffer({
            {ATT_GBUFFER_ALBEDO, GL_RGBA8},
            {ATT_GBUFFER_DEPTH,  GL_R16F}   
        }),
        hdrFbo({{GL_COLOR_ATTACHMENT0, GL_RGBA16F}}),
        sdrFbo1({{GL_COLOR_ATTACHMENT0, GL_RGBA8}}),
        sdrFbo2({{GL_COLOR_ATTACHMENT0, GL_RGBA8}})
    {
        GL::Framebuffer::Default.BindForDrawing();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Renderer::ResizeFramebuffers(const glm::ivec2 &size) {
        gbuffer.Resize(size);
        hdrFbo.Resize(size);
        sdrFbo1.Resize(size);
        sdrFbo2.Resize(size);
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::BeginGeometry() {
        gbuffer.BindForDrawing();
    }

    void Renderer::EndGeometry() {
        rlDrawRenderBatchActive();
        GL::Framebuffer::Default.BindForDrawing();
    }

    void Renderer::Draw(const DrawQueue &dq, const Camera2D &camera) {

        // ==================== SETUP ====================

        // Ensure we don't mess up raylib internal state
        rlDrawRenderBatchActive();

        // Defaults
        glDisable(GL_CULL_FACE);

        // Calculate matrices
        glm::mat4 viewMatrix = ToGLMMatrix(GetCameraMatrix2D(camera));
        glm::mat4 projectionMatrix = 
            glm::ortho(
                0.0f, (float) GetScreenWidth(), 
                (float) GetScreenHeight(), 0.0f, 
               -1.0f, 1.0f
            ) *
            ToGLMMatrix(MatrixScale(
                GL::Framebuffer::Default.Size().x/(1.0f*GetScreenWidth()),  
                GL::Framebuffer::Default.Size().y/(1.0f*GetScreenHeight()), 
                1.0f 
            ));
        glm::mat4 viewProjection = projectionMatrix * viewMatrix;

        // Bind textures
        TEX_ALBEDO_MAP.AttachTexture2D(gbuffer.GetTexture(ATT_GBUFFER_ALBEDO));
        TEX_DEPTH_MAP .AttachTexture2D(gbuffer.GetTexture(ATT_GBUFFER_DEPTH));
        TEX_HDR       .AttachTexture2D(hdrFbo.GetTexture(GL_COLOR_ATTACHMENT0));
        TEX_SDR1      .AttachTexture2D(sdrFbo1.GetTexture(GL_COLOR_ATTACHMENT0));
        TEX_SDR2      .AttachTexture2D(sdrFbo2.GetTexture(GL_COLOR_ATTACHMENT0));

        // Ping-pong framebuffers
        GL::Framebuffer *sdrFboSrc = &sdrFbo1, *sdrFboDst = &sdrFbo2;
        GL::TextureUnit *TEX_SDR_SRC = &TEX_SDR1, *TEX_SDR_DST = &TEX_SDR2;

        // ==================== RENDERING ====================

        // Apply lighting to gbuffer
        hdrFbo.BindForDrawing();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        lightRenderer.DrawLights(
            viewProjection, TEX_ALBEDO_MAP, TEX_DEPTH_MAP, 
            dq.ambientLight, 
            dq.lights.data(), dq.lights.size(),
            dq.energySpheres.data(), dq.energySpheres.size()
        );


        // Apply tone mapping
        sdrFboDst->BindForDrawing();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        toneMappingProgram()
            .SetUniform("tex", TEX_HDR)
            .SetUniform("exposure", dq.exposure)
            .DrawPostprocessing();

        std::swap(sdrFboSrc, sdrFboDst);
        std::swap(TEX_SDR_SRC, TEX_SDR_DST);

        // Apply distortions

        sdrFboSrc->CopyTo(GL::Framebuffer::Default);
        distortionRenderer.DrawDistortions(
            viewProjection, *TEX_SDR_SRC, 
            dq.shockwaves.data(), dq.shockwaves.size()
        );

        // ==================== CLEANUP ====================
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}