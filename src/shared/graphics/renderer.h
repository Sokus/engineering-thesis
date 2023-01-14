#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <vector>
#include <glm/vec2.hpp>

#include <gl/framebuffer.h>
#include "light.h"

namespace Game {

    struct Renderer;

    struct DrawQueue {

        friend struct Renderer;

        private:
        std::vector<Light> lights;

        public:

        void DrawLight(const Light &light);
        void Clear();
    };
    
    struct Renderer {

        private:

        GL::Framebuffer gbuffer;

        public:

        Renderer();

        /* Anything rendered using glDrawXXX between BeginGeometry() and EndGeometry()
           gets captured into the gbuffer
        */
        void BeginGeometry();
        void EndGeometry();

        void ResizeFramebuffers(const glm::ivec2 &size);
        void Draw(const DrawQueue &);
    };

}

#endif