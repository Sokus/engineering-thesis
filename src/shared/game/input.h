#ifndef INPUT_H
#define INPUT_H

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
    bool sprint;
    int cooldown;


    void Update();
};

} // namespace Game
#endif