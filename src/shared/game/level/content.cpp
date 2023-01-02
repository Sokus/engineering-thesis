#include "content.h"

#include <raylib.h>

namespace Game {

    Level plains;
    Texture2D bulletTexture;
    std::vector<Level> levels = { plains };

    void InitGameContent() {
        plains = Level(plainsTiles,plainsMovingTiles,plainsInteractiveTiles,plainsCollectibles,plainsDamagingTiles,plainTextures);
        
        Game::bulletTexture = LoadTexture(RESOURCE_PATH "/ballLightning.2x2.png");
        Game::ParallaxLayer hills, clouds, sky;

        hills.dimensions = {960,540};
        hills.position = {0,0,3};
        hills.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/hills.png");
        hills.naturalScrollSpeed = -300;

        clouds.dimensions = {960,540};
        clouds.position = {0,0,10};
        clouds.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/clouds.png");
        clouds.naturalScrollSpeed = -300;
        
        sky.dimensions = {960,540};
        sky.position = {0,0,999};
        sky.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/sky.png");
        sky.naturalScrollSpeed = 0;

        plains.AddParallaxLayer(hills);
        plains.AddParallaxLayer(clouds);
        plains.AddParallaxLayer(sky);
    }
}