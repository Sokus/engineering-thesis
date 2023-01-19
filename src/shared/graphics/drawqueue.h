#ifndef GRAPHICS_DRAWQUEUE_H
#define GRAPHICS_DRAWQUEUE_H

#include <vector>
#include <glm/glm.hpp>
#include "light.h"
#include "distortion.h"

namespace Game {

    struct Renderer;

    struct DrawQueue {

        friend struct Renderer;

        private:
        std::vector<Light> lights;
        std::vector<Light> energySpheres;
        std::vector<ShockwaveEffect> shockwaves;

        public:

        glm::vec3 ambientLight = glm::vec3(0.025f) * 10.0f;
        float exposure = 1.2f;
        
        float grayscaleEffectStrength = 0; //0 - 1

        glm::vec4 vignetteColor = glm::vec4(0);
        float vignetteExponent = 0.25f;

        float blurStrength = 0;            //0 - N

        void DrawLight(const Light &light);
        void DrawEnergySphere(const Light &sphere);
        void DrawShockwaveEffect(const ShockwaveEffect &effect);
        void Clear();
    };

}

#endif