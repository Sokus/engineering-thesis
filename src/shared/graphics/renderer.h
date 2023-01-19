#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <vector>
#include <glm/vec2.hpp>
#include <raylib.h>

#include <gl/framebuffer.h>
#include "drawqueue.h"
#include "light.h"
#include "distortion.h"

namespace Game {
    
    struct Renderer {

        private:

        GL::Framebuffer gbuffer;
        GL::Framebuffer hdrFbo;
        GL::Framebuffer sdrFbo1, sdrFbo2;

        LightRenderer lightRenderer;
        DistortionRenderer distortionRenderer;

        public:

        Renderer();

        /* Anything rendered using glDrawXXX between BeginGeometry() and EndGeometry()
           gets captured into the gbuffer
        */
        void BeginGeometry();
        void EndGeometry();

        void ResizeFramebuffers(const glm::ivec2 &size);
        void Draw(const DrawQueue &, const Camera2D &);
    };

}

#endif