#ifndef SDL2_PLATFORM_H
#define SDL2_PLATFORM_H

#include <stdint.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"

#define DEFAULT_KEY_UP SDLK_w
#define DEFAULT_KEY_LEFT SDLK_a
#define DEFAULT_KEY_DOWN SDLK_s
#define DEFAULT_KEY_RIGHT SDLK_d

#define SDL2_KEYCODE_OFFSET (-0x40000039 + 0x80)
#define SDL2_INPUT_KEY_COUNT 354

namespace OS {

struct SDL2_Input
{
    bool keys_down[SDL2_INPUT_KEY_COUNT];
    float keys_down_duration[SDL2_INPUT_KEY_COUNT];
    float keys_down_duration_previous[SDL2_INPUT_KEY_COUNT];

    void Init();
    void Update(float dt);
    void SetByKeycode(SDL_Keycode keycode, bool value);
};

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

static void SDL2_SetContext(SDL2_Context *ctx);
static void SDL2_InitSDL(SDL2_Context *ctx,
                         const char *window_title,
                         int screen_width, int screen_height);
static void SDL2_InitGL(SDL2_Context *ctx);
static void SDL2_InitImGui(SDL2_Context *ctx);


SDL2_Context *SDL2_GetContext();

void CreateContext(char *window_title, int screen_width, int screen_height);
void DestroyContext();
void BeginFrame();
void EndFrame();
float GetSecondsElapsed(uint64_t start_counter, uint64_t end_counter);
float DeltaTime();
bool IsRunning();
void GetWindowDimensions(int *screen_width, int *screen_height);

void ProcessEvent(const SDL_Event* event);

} // namespace OS

#endif // SDL2_PLATFORM_H