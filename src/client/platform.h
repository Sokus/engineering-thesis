#ifndef PI_PLATFORM_H
#define PI_PLATFORM_H

#include <stdint.h>

#if defined(PLATFORM_WINDOWS) & !defined(SDL_MAIN_HANDLED)
    #define SDL_MAIN_HANDLED
#endif
#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"

#include "config.h"
#include "input.h"

namespace OS {

// Holds all data required for running SDL_GL context
struct SDL2_Context
{
    SDL_Window *window;
    SDL_GLContext gl_context;
    Uint64 last_performance_counter;

    SDL2_Input input;

    const char *window_title;
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
static void SDL2_ProcessEvent(const SDL_Event* event);

void CreateContext(const char *window_title, int screen_width, int screen_height);
void DestroyContext();
void BeginFrame();
void EndFrame();
uint64_t GetPerformanceCounter();
float GetSecondsElapsed(uint64_t start_counter, uint64_t end_counter);
float DeltaTime();
bool IsRunning();
void GetWindowDimensions(int *screen_width, int *screen_height);


} // namespace OS

#endif // PI_PLATFORM_H