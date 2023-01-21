#include "particles.h"

namespace Game::Particles {

    ParticleType 
        *pebble,
        *spark;

    void InitContentTypes() {

        pebble = particleRegistry.CreateParticleType();
        pebble->textureBounds = {0.0f, 0.0f, 16.0f, 16.0f};
        pebble->gravity = 240;

        spark = particleRegistry.CreateParticleType();
        spark->textureBounds = {16.0f, 0.0f, 16.0f, 16.0f};
    }
}