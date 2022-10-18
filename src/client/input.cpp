#include "input.h"
#include "macros.h"
#include "platform.h"

#include <string.h> // memset, memcpy

namespace Input {

InputState input = {};

void BeginFrame()
{
    memcpy(input.keys_down_previous, input.keys_down, sizeof(input.keys_down));
}

void SetByKeycode(SDL_Keycode keycode, bool value)
{
    // SDL Keycodes:
    // can be represented:    from 0x00 to 0x7F, 0x80 total (128)
    // cannot be represented: 0x40000039 - 0x4000011A, 0xE2 total (226)
    // 128 + 226 = 354
    const unsigned int keycode_offset = -0x40000039 + 0x80;
    unsigned int index;
    index = (keycode <= 0x7F) ? keycode : (keycode + keycode_offset);
    input.keys_down[index] = value;
}

bool KeyDown(int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input.keys_down));
    bool result = input.keys_down[key_index];
    return result;
}

bool KeyPressed(int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input.keys_down));
    bool result = !input.keys_down_previous[key_index] && input.keys_down[key_index];
    return result;
}

bool KeyReleased(int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input.keys_down));
    bool result = input.keys_down_previous[key_index] && !input.keys_down[key_index];
    return result;
}

}