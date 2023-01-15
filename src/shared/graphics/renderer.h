#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <vector>
#include <glm/vec2.hpp>
#include <raylib.h>

#include <gl/framebuffer.h>
#include "light.h"

namespace Game {

    struct Renderer;

    struct DrawQueue {

        friend struct Renderer;

        private:
        std::vector<Light> lights;

        public:

        glm::vec3 ambientLight = glm::vec3(0.5f);
        void DrawLight(const Light &light);
        void Clear();
    };
    
    struct Renderer {

        private:

        GL::Framebuffer gbuffer;
        GL::Framebuffer hdrFbo;

        LightRenderer lightRenderer;

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