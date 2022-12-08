#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace Game {

    /// Scrolling background image. Used for achieving a parallax scrolling effect. 
    struct ParallaxLayer
    {  
        Texture2D texture;
        glm::vec3 position;
        glm::vec2 dimensions;

        /// How fast the layer scrolls horizontally even when the camera is not moving
        float naturalScrollSpeed = 0;
        bool repeatHorizontally = true;

        void Update(float dt);
        void Draw(glm::vec2 cameraPosition) const;

        private:
        float horizontalOffset = 0;
    };

    class Level
    {
    private:

        std::vector<ParallaxLayer> parallaxLayers;
        std::vector<float> layerOffsets;
        bool layersSorted = false;

    public:

        void AddParallaxLayer(const ParallaxLayer &layer);

        void Update(float dt);
        void DrawBackground(glm::vec2 cameraPosition) const;
    };
}

#endif