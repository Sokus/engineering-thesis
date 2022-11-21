#ifndef INPUT_H
#define INPUT_H

namespace Game {

struct Input
{
    enum Direction
    {
        UP    = 0,
        LEFT  = 1,
        DOWN  = 2,
        RIGHT = 3,
        SHIFT = 4,
        INTERACT = 5,
        COUNT = 6
    };

    bool move[Direction::COUNT];
    bool jump;

    void Update();
};

} // namespace Game
#endif