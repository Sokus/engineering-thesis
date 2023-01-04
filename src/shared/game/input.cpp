#include "input.h"
#include "serialization/serialize.h"
#include "serialization/bitpacker.h"

#include "raylib.h"
#include <stdio.h>

namespace Game {

void Input::Update()
{
    move[DIRECTION_UP] = IsKeyDown(KEY_W);
    move[DIRECTION_LEFT] = IsKeyDown(KEY_A);
    move[DIRECTION_DOWN] = IsKeyDown(KEY_S);
    move[DIRECTION_RIGHT] = IsKeyDown(KEY_D);
    dash = IsKeyDown(KEY_C);
    interact = IsKeyDown(KEY_I);
    shoot = IsKeyDown(KEY_SPACE);
}

bool SerializeInput(::BitPacker *bit_packer, Input *input)
{
    for(int i = 0; i < Input::Direction::DIRECTION_COUNT; i++)
        SERIALIZE_BOOL(bit_packer, &input->move[i]);
    SERIALIZE_BOOL(bit_packer, &input->jump);
    return true;
}

} // namespace Game