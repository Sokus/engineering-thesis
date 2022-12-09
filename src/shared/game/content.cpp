#include "content.h"

#include <raylib.h>

namespace Game {

    BulletType exampleBullet;

    Level exampleLevel;

    void InitGameContent() {
        exampleBullet.maxLifetime = 1.5;
        exampleBullet.visibleSize = {50,50};
        exampleBullet.texture = LoadTexture(RESOURCE_PATH "/ballLightning.2x2.png");
        exampleBullet.animationFrames = {2,2};
        exampleBullet.animationLength = 0.5;
        exampleBullet.sizeKeyframes = {0.5, 1.5, 0.5};
        exampleBullet.alphaKeyframes = {1, 1, 0};
        exampleBullet.SetDrag(0.5);
        exampleBullet.gravity = -200;

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

        exampleLevel.AddParallaxLayer(hills);
        exampleLevel.AddParallaxLayer(clouds);
        exampleLevel.AddParallaxLayer(sky);
    }

}