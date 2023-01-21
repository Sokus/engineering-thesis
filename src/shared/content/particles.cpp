#include "particles.h"

namespace Game::Particles {

    ParticleType 
        *pebble,
        *spark,
        *blood,
        *alienBlood,
        *bone;

    void InitContentTypes() {

        pebble = particleRegistry.CreateParticleType();
        pebble->textureBounds = {0.0f, 0.0f, 16.0f, 16.0f};
        pebble->gravity = 240;

        spark = particleRegistry.CreateParticleType();
        spark->textureBounds = {16.0f, 0.0f, 16.0f, 16.0f};

        blood = particleRegistry.CopyParticleType(pebble);
        blood->textureBounds = {32.0f, 0.0f, 16.0f, 16.0f};

        alienBlood = particleRegistry.CopyParticleType(pebble);
        alienBlood->textureBounds = {0.0f, 16.0f, 16.0f, 16.0f};

        bone = particleRegistry.CopyParticleType(pebble);
        bone->textureBounds = {16.0f, 16.0f, 16.0f, 16.0f};
    }
}