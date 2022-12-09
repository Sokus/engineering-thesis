#include "content.h"

#include <raylib.h>

namespace Game {

    BulletType exampleBullet;

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
    }

}