#include "input.h"

#include "raylib.h"
#include <stdio.h>


namespace Game {

Input GetInput()
{
    Input input = {};
    input.move[DIRECTION_UP] = IsKeyDown(KEY_W);
    input.move[DIRECTION_LEFT] = IsKeyDown(KEY_A);
    input.move[DIRECTION_DOWN] = IsKeyDown(KEY_S);
    input.move[DIRECTION_RIGHT] = IsKeyDown(KEY_D);
    input.dash = IsKeyDown(KEY_C);
    input.interact = IsKeyDown(KEY_I);
    input.shoot = IsKeyDown(KEY_SPACE);
    return input;
}

} // namespace Game