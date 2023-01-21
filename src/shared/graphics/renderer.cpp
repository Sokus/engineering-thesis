#include "renderer.h"

#include <stdio.h>
#include <rlgl.h>
#include <raymath.h>
#include <gl/gl.h>
#include <gl/texture.h>
#include "programs.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "kernel.h"

namespace Game {

    enum {
        ATT_GBUFFER_ALBEDO = GL_COLOR_ATTACHMENT0,
        ATT_GBUFFER_DEPTH = GL_COLOR_ATTACHMENT1,
        ATT_GBUFFER_EMISSIVE = GL_COLOR_ATTACHMENT2
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
        TEX_EMISSIVE_MAP(9),
        TEX_HDR(2),
        TEX_SDR1(3),
        TEX_SDR2(4),
        TEX_BLOOM[] {{5}, {6}, {7}, {8}};


    Renderer::Renderer() :
        gbuffer({
            {ATT_GBUFFER_ALBEDO, GL_RGBA8},
            {ATT_GBUFFER_DEPTH,  GL_R16F},
            {ATT_GBUFFER_EMISSIVE, GL_RGB16F}   
        }),
        hdrFbo({{GL_COLOR_ATTACHMENT0, GL_RGBA16F}}),
        sdrFbo1({{GL_COLOR_ATTACHMENT0, GL_RGBA8}}),
        sdrFbo2({{GL_COLOR_ATTACHMENT0, GL_RGBA8}})
    {
        for(int i=0; i<4; ++i) {
            int downscale = 2 << i;
            bloomFbos.push_back(GL::Framebuffer({{GL_COLOR_ATTACHMENT0, GL_RGB16F}}, gbuffer.Size()/downscale));
        }
        GL::Framebuffer::Default.BindForDrawing();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Renderer::ResizeFramebuffers(const glm::ivec2 &size) {
        gbuffer.Resize(size);
        hdrFbo.Resize(size);
        sdrFbo1.Resize(size);
        sdrFbo2.Resize(size);
        for(int i=0; i<bloomFbos.size(); ++i) {
            int downscale = 2 << i;
            bloomFbos[i].Resize(size/downscale);
        }
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
        TEX_EMISSIVE_MAP.AttachTexture2D(gbuffer.GetTexture(ATT_GBUFFER_EMISSIVE));
        TEX_HDR       .AttachTexture2D(hdrFbo.GetTexture(GL_COLOR_ATTACHMENT0));
        TEX_SDR1      .AttachTexture2D(sdrFbo1.GetTexture(GL_COLOR_ATTACHMENT0));
        TEX_SDR2      .AttachTexture2D(sdrFbo2.GetTexture(GL_COLOR_ATTACHMENT0));
        for(int i=0; i<bloomFbos.size(); ++i)
            TEX_BLOOM[i].AttachTexture2D(bloomFbos[i].GetTexture(GL_COLOR_ATTACHMENT0));

        // Ping-pong framebuffers
        GL::Framebuffer *sdrFboSrc = &sdrFbo1, *sdrFboDst = &sdrFbo2;
        GL::TextureUnit *TEX_SDR_SRC = &TEX_SDR1, *TEX_SDR_DST = &TEX_SDR2;

        // ==================== RENDERING ====================

        // Apply lighting to gbuffer
        hdrFbo.BindForDrawing();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        lightRenderer.DrawLights(
            viewProjection, 
            TEX_ALBEDO_MAP, TEX_DEPTH_MAP, TEX_EMISSIVE_MAP,
            dq.ambientLight, 
            dq.lights.data(), dq.lights.size(),
            dq.energySpheres.data(), dq.energySpheres.size()
        );

        // Bloom
        if(dq.bloomStrength > 0) {

            // Downsample bloom fbos
            for(int i=0; i<bloomFbos.size(); ++i) {
                bloomFbos[i].BindForDrawing();
                blur3x3Program()
                    .SetUniform("tex", i == 0 ? TEX_HDR : TEX_BLOOM[i-1])
                    .DrawPostprocessing();
            }

            // Upsample bloom fbos
            for(int i=bloomFbos.size()-2; i>=0; --i) {
                bloomFbos[i].BindForDrawing();
                blur3x3Program()
                    .SetUniform("tex", TEX_BLOOM[i+1])
                    .DrawPostprocessing();
            }

            // Write bloom back to HDR fbo
            hdrFbo.BindForDrawing();
            bloomApplyProgram()
                .SetUniform("tex", TEX_BLOOM[0])
                .SetUniform("bloomStrength", dq.bloomStrength)
                .DrawPostprocessing();
        }

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

        // Apply distortions (shockwaves etc.)

        sdrFboSrc->CopyTo(*sdrFboDst);
        distortionRenderer.DrawDistortions(
            viewProjection, *TEX_SDR_SRC, 
            dq.shockwaves.data(), dq.shockwaves.size()
        );

        std::swap(sdrFboSrc, sdrFboDst);
        std::swap(TEX_SDR_SRC, TEX_SDR_DST);

        // Apply grayscale effect
        
        if(dq.grayscaleEffectStrength > 1e-4) {
            sdrFboDst->BindForDrawing();
            glClear(GL_COLOR_BUFFER_BIT);
            grayscaleProgram()
                .SetUniform("tex", *TEX_SDR_SRC)
                .SetUniform("strength", dq.grayscaleEffectStrength)
                .DrawPostprocessing();
            std::swap(sdrFboSrc, sdrFboDst);
            std::swap(TEX_SDR_SRC, TEX_SDR_DST);
        }

        // Apply blur
        if(dq.blurStrength > 1e-4) {
            auto kernel = Kernel1D::GaussianBlur(dq.blurStrength);
            kernel.Apply(*sdrFboDst, *TEX_SDR_SRC, glm::vec2(1,0));
            kernel.Apply(*sdrFboSrc, *TEX_SDR_DST, glm::vec2(0,1));
        }

        // Copy results to default FBO
        sdrFboSrc->CopyTo(GL::Framebuffer::Default);

        // Apply vignette effect
        if(dq.vignetteColor.a > 0) {
            vignetteProgram()
                .SetUniform("vignetteColor", dq.vignetteColor)
                .SetUniform("vignetteExponent", dq.vignetteExponent)
                .DrawPostprocessing();
        }

        // ==================== CLEANUP ====================
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}