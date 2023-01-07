#include "input.h"
#include "serialization/bitstream.h"
#include "serialization/serialize.h"

#include <stdio.h>

#include "raylib.h"

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

bool Input::Serialize(BitStream *stream)
{
    for (int i = 0; i < DIRECTION_COUNT; i++)
        SERIALIZE_BOOL(stream, move[i]);
    SERIALIZE_BOOL(stream, jump);
    SERIALIZE_BOOL(stream, shoot);
    SERIALIZE_BOOL(stream, interact);
    SERIALIZE_BOOL(stream, dash);
    return true;
}

} // namespace Game