#ifndef INPUT_H
#define INPUT_H

#include "serialization/serialization.h"

namespace Game {

struct Input
{
    Input();
    enum Direction
    {
        UP    = 0,
        LEFT  = 1,
        DOWN  = 2,
        RIGHT = 3,
        COUNT = 4
    };

    bool move[Direction::COUNT];
    bool jump;
    bool shoot;
    bool interact;
    bool dash;
    int cooldown;


    void Update();
};

bool SerializeInput(BitPacker *bit_packer, Input *input);

} // namespace Game
#endif