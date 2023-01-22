#ifndef GRAPHICS_RAYLIB_SHADERS_H
#define GRAPHICS_RAYLIB_SHADERS_H

#include <raylib.h>

namespace Game::RaylibShaders {
    
    extern Shader world;
    void worldSetDepth(float value);
    void worldSetEmissiveness(float value);

    extern Shader particle;
    void particleSetEmissiveTexture(Texture2D texture);

    void LoadShaders();
    void UnloadShaders();
}

#endif