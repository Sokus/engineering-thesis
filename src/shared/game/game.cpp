#include "macros.h"
#include "config.h"

#include "glm/glm.hpp"
#include "raylib.h"

namespace Game {


    Texture2D character;
    Texture2D tile;




void LoadTextures()
{
    character = LoadTexture(RESOURCE_PATH "/character.png");
    tile = LoadTexture(RESOURCE_PATH "/tile.png");
}


} // namespace Game