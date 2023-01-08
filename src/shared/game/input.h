#ifndef INPUT_H
#define INPUT_H

struct BitStream;

namespace Game {

enum InputDirection
{
    DIRECTION_UP    = 0,
    DIRECTION_LEFT  = 1,
    DIRECTION_DOWN  = 2,
    DIRECTION_RIGHT = 3,
    DIRECTION_COUNT = 4
};

struct Input
{
    bool move[DIRECTION_COUNT];
    bool jump;
    bool shoot;
    bool interact;
    bool dash;

    bool Serialize(BitStream *stream);
};

Input GetInput();


} // namespace Game
#endif