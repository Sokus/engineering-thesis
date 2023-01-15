#ifndef GRAPHICS_RAYLIB_SHADERS_H
#define GRAPHICS_RAYLIB_SHADERS_H

#include <raylib.h>

namespace Game::RaylibShaders {
    
    extern Shader world;
    extern int worldDepthLoc;

    void LoadShaders();
    void UnloadShaders();
}

#endif