#include "input.h"
#include "serialization/serialization.h"

#include "raylib.h"
#include <stdio.h>

namespace Game {
    Input::Input() {
        cooldown = 0;
    }

void Input::Update()
{
    move[UP] = IsKeyDown(KEY_W);
    move[LEFT] = IsKeyDown(KEY_A);
    move[DOWN] = IsKeyDown(KEY_S);
    move[RIGHT] = IsKeyDown(KEY_D);
    dash = IsKeyDown(KEY_C);
    interact = IsKeyDown(KEY_I);
    shoot = IsKeyDown(KEY_SPACE);
    if (cooldown <= 90) {
        cooldown += 1;
    }
}

bool SerializeInput(BitPacker *bit_packer, Input *input)
{
    for(int i = 0; i < Input::Direction::COUNT; i++)
        SERIALIZE_BOOL(bit_packer, &input->move[i]);
    SERIALIZE_BOOL(bit_packer, &input->jump);
    return true;
}

} // namespace Game