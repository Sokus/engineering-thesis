#ifndef LEVEL_H
#define LEVEL_H

namespace Game {

enum LevelType
{
    LEVEL_INVALID,
    LEVEL_PLAINS,
    LEVEL_LAKE,
    LEVEL_CLOUDS,
    // ---------
    LEVEL_COUNT,
};

struct World;
void InitLevel(World *world, LevelType type);
void SetLoadTextures(bool value);

}

#endif // LEVEL_H