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

        // Passive light present even without light sources. Any positive values are allowed.
        glm::vec3 ambientLight = glm::vec3(0.025f) * 10.0f;

        // Larger values = stronger bloom
        float bloomStrength = 0.01;

        // Exposure time for tone mapping. Larger values improve visibility in dark environments.
        float exposure = 1.2f; 
        
        // Reduces saturation of the image
        float grayscaleEffectStrength = 0;

        // Controls the size of the vignette effect. Larger values = larger vignette.
        float vignetteExponent = 0.25f;
        glm::vec4 vignetteColor = glm::vec4(0);

        // Larger values = more blurry image
        float blurStrength = 0;

        void DrawLight(const Light &light);
        void DrawEnergySphere(const Light &sphere);
        void DrawShockwaveEffect(const ShockwaveEffect &effect);
        void Clear();
    };

}

#endif