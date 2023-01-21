#ifndef GRAPHICS_RAYLIB_SHADERS_H
#define GRAPHICS_RAYLIB_SHADERS_H

#include <raylib.h>

namespace Game::RaylibShaders {
    
    extern Shader world;
    void worldSetDepth(float value);
    void worldSetEmissiveness(float value);

    void LoadShaders();
    void UnloadShaders();
}

#endif