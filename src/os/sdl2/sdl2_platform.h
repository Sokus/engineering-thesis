#ifndef SDL2_PLATFORM_H
#define SDL2_PLATFORM_H

#include <stdint.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"

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

// Holds all data required for running SDL_GL context
struct SDL2_Context
{
    SDL_Window *window;
    SDL_GLContext gl_context;
    Uint64 last_performance_counter;

    SDL2_Input input;

    char *window_title;
    int screen_width;
    int screen_height;
    bool is_running;
    bool is_fullscreen;
    float target_fps;
    float dt;
};

SDL2_Context *SDL2_GetContext();
static void SDL2_InitSDL(SDL2_Context *ctx,
                         const char *window_title,
                         int screen_width, int screen_height);
static void SDL2_InitGL(SDL2_Context *ctx);
static void SDL2_InitImGui(SDL2_Context *ctx);
void CreateContext(char *window_title, int screen_width, int screen_height);
void DestroyContext();
void BeginFrame();
void EndFrame();
uint64_t GetPerformanceCounter();
float GetSecondsElapsed(uint64_t start_counter, uint64_t end_counter);
float DeltaTime();
bool IsRunning();
void GetWindowDimensions(int *screen_width, int *screen_height);

static void SDL2_ProcessEvent(const SDL_Event* event);

} // namespace OS

#endif // SDL2_PLATFORM_H