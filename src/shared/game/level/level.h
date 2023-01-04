#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <vector>
#include <string>

namespace Game {

    /// Scrolling background image. Used for achieving a parallax scrolling effect.
    struct ParallaxLayer
    {
        Texture2D texture;
        Vector3 position;
        Vector2 dimensions;

        /// How fast the layer scrolls horizontally even when the camera is not moving
        float naturalScrollSpeed = 0;
        bool repeatHorizontally = true;

        void Update(float dt);
        void Draw(Vector2 cameraPosition) const;

        private:
        float horizontalOffset = 0;
    };
    struct TileData {
        TileData(float positionx, float positiony,float size_x,float size_y, int connGroup, int text);
        TileData(float positionx, float positiony,float size_x,float size_y, int connGroup, int text, Vector2 velocity, Vector2 border[2]);
        Vector2 position;
        Vector2 size;
        int connGroup;
        Vector2 border[2];
        Vector2 velocity;
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
        Level(Vector2 spawnpoint,
            std::vector<TileData> tiles,
            std::vector<TileData> movingTiles,
            std::vector<TileData> interactiveTiles,
            std::vector<TileData> collectibles,
            std::vector<TileData> damagingTiles,
            std::vector<TileData> destroyTiles,
            std::vector<TileData> checkpoints,
            std::vector<TileData> levelExit,
            std::vector<std::string> textures
        );
        void AddParallaxLayer(const ParallaxLayer &layer);
        void LoadTextures();
        void Update(float dt);
        void DrawBackground(Vector2 cameraPosition) const;
        Vector2 spawnpoint;
        std::vector<TileData> tiles;
        std::vector<TileData> movingTiles;
        std::vector<TileData> interactiveTiles;
        std::vector<TileData> collectibles;
        std::vector<TileData> damagingTiles;
        std::vector<TileData> destroyTiles;
        std::vector<TileData> checkpoints;
        std::vector<TileData> levelExits;
        std::vector<std::string> textures;
        std::vector<Texture2D> textures2d;
        bool finished;
    };
}

#endif