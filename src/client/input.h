#ifndef INPUT_H
#define INPUT_H

#include "config.h"

#if defined(PLATFORM_WINDOWS) & !defined(SDL_MAIN_HANDLED)
    #define SDL_MAIN_HANDLED
#endif
#include "SDL.h"

namespace Input {

#define INPUT_KEY_COUNT 354

enum Keybind
{
    Left = SDLK_a,
    Right = SDLK_d,
    Up = SDLK_w,
    Down = SDLK_s
};

struct InputState
{
    bool keys_down[INPUT_KEY_COUNT];
    bool keys_down_previous[INPUT_KEY_COUNT];
};

void BeginFrame();
void SetByKeycode(SDL_Keycode keycode, bool value);

bool KeyDown(int key_index);
bool KeyPressed(int key_index);
bool KeyReleased(int key_index);

} // namespace Input

#endif //INPUT_H
