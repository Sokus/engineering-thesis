#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

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
    struct TileData {
        TileData(float positionx, float positiony, int connGroup, int text);
        TileData(float positionx, float positiony, int connGroup, int text, glm::vec2 velocity, glm::vec2 border[2]);
        glm::vec2 position;
        int connGroup;
        glm::vec2 border[2];
        glm::vec2 velocity;
        int texture;
    };

    class Level
    {
    private:

        std::vector<ParallaxLayer> parallaxLayers;
        std::vector<float> layerOffsets;
        bool layersSorted = false;

    public:
        Level() {

        };
        Level(std::vector<TileData> tiles,
            std::vector<TileData> movingTiles,
            std::vector<TileData> interactiveTiles,
            std::vector<TileData> collectibles,
            std::vector<TileData> damagingTiles,
            std::vector<TileData> destroyTiles,
            std::vector<std::string> textures
        );
        void AddParallaxLayer(const ParallaxLayer &layer);
        void LoadTextures();
        void Update(float dt);
        void DrawBackground(glm::vec2 cameraPosition) const;
        std::vector<TileData> tiles;
        std::vector<TileData> movingTiles;
        std::vector<TileData> interactiveTiles;
        std::vector<TileData> collectibles;
        std::vector<TileData> damagingTiles;
        std::vector<TileData> destroyTiles;
        std::vector<std::string> textures;
        std::vector<Texture2D> textures2d;
    };
}

#endif