#ifndef INPUT_H
#define INPUT_H

namespace Game {

struct Input
{
    enum Direction
    {
        DIRECTION_UP    = 0,
        DIRECTION_LEFT  = 1,
        DIRECTION_DOWN  = 2,
        DIRECTION_RIGHT = 3,
        DIRECTION_COUNT = 4
    };

    bool move[DIRECTION_COUNT];
    bool jump;
    bool shoot;
    bool interact;
    bool dash;

    void Update();
};

struct BitPacker;
bool SerializeInput(BitPacker *bit_packer, Input *input);

} // namespace Game
#endif