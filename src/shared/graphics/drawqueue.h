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
        std::vector<Light> energySpheres;

        public:

        glm::vec3 ambientLight = glm::vec3(0.025f) * 10.0f;
        float exposure = 1.2f;

        void DrawLight(const Light &light);
        void DrawEnergySphere(const Light &sphere);
        void Clear();
    };

}

#endif