#ifndef INPUT_H
#define INPUT_H

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include "SDL.h"

namespace OS {

// SDL Keycodes:
// can be represented:    from 0x00 to 0x7F, 0x80 total (128)
// cannot be represented: 0x40000039 - 0x4000011A, 0xE2 total (226)
// 128 + 226 = 354

#define SDL2_KEYCODE_OFFSET (-0x40000039 + 0x80)
#define SDL2_INPUT_KEY_COUNT 354

enum Keybind
{
    Up = SDLK_w,
    Left = SDLK_a,
    Down = SDLK_s,
    Right = SDLK_d,
};

// Keeps input data like button state and keypress duration
struct SDL2_Input
{
    bool keys_down[SDL2_INPUT_KEY_COUNT];
    float keys_down_duration[SDL2_INPUT_KEY_COUNT];
    float keys_down_duration_previous[SDL2_INPUT_KEY_COUNT];

    void Init();
    void Update(float dt);
    void SetByKeycode(SDL_Keycode keycode, bool value);
};

bool IsKeyDown(int key_index);
bool WasKeyDown(int key_index);
bool IsKeyPressed(int key_index);

} // namespace OS

#endif //INPUT_H
