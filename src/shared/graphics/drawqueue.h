#ifndef GRAPHICS_DRAWQUEUE_H
#define GRAPHICS_DRAWQUEUE_H

#include <vector>
#include <glm/glm.hpp>
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

}

#endif