#ifndef CONTENT_PARTICLES_H
#define CONTENT_PARTICLES_H

#include <graphics/particle.h>

namespace Game::Particles {

    extern ParticleType 
        *pebble,
        *spark;

    void InitContentTypes();
}

#endif