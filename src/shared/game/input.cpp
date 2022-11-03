#include "input.h"

#include "raylib.h"

namespace Game {

void Input::Update()
{
    move[UP] = IsKeyDown(KEY_W);
    move[LEFT] = IsKeyDown(KEY_A);
    move[DOWN] = IsKeyDown(KEY_S);
    move[RIGHT] = IsKeyDown(KEY_D);

    jump = IsKeyDown(KEY_SPACE);
}

} // namespace Game