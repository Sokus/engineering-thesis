#ifndef LEVEL_H
#define LEVEL_H

namespace Game {

enum LevelType
{
    LEVEL_INVALID,
    LEVEL_PLAINS,
};

struct World;
void InitLevel(World *world, LevelType type);

}

#endif // LEVEL_H