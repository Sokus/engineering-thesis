#include <stdint.h>
#include <stdio.h>

#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "platform.h"
#include "macros.h"

namespace OS {

SDL2_Context *g_sdl2_ctx = nullptr;

/**
 * Create platform context, create windows, initialize subsystems. It also
 * sets some default parameters, like window size or target framerates.
 *
 * This MUST be called before anything related
 * to SDL/OpenGL/DearImGui is done.
 */
void CreateContext(const char* window_title,
                   int screen_width,
                   int screen_height)
{
    SDL2_Context *ctx = (SDL2_Context *)malloc(sizeof(SDL2_Context));
    g_sdl2_ctx = ctx;

    SDL2_InitSDL(ctx, window_title, screen_width, screen_height);
    SDL2_InitGL(ctx);
    SDL2_InitImGui(ctx);

    ctx->last_performance_counter = SDL_GetPerformanceCounter();
    ctx->is_running = true;
    ctx->window_title = window_title;
    ctx->screen_width = screen_width;
    ctx->screen_height = screen_height;
    float target_fps = 60.0f;
    ctx->target_fps = target_fps;
    ctx->dt = 1.0f / target_fps;
}

/**
 * Cleanup SDL/OpenGL/DearImGui contexts, called at the end of the program.
 */
void DestroyContext()
{
    SDL2_Context *ctx = SDL2_GetContext();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(ctx->gl_context);
    SDL_DestroyWindow(ctx->window);
    SDL_Quit();
    free(ctx);
}

/**
 * Poll and process window/io events, update DearImGui, get new
 * window dimensions if changed, update input state.
 */
void BeginFrame()
{
    SDL2_Context *ctx = SDL2_GetContext();

    Input::BeginFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        SDL2_ProcessEvent(&event);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    SDL_GetWindowSize(ctx->window,
                      &ctx->screen_width,
                      &ctx->screen_height);

    glViewport(0, 0, ctx->screen_width, ctx->screen_height);
}

/**
 * Render ImGuiDrawData, swap SDL window backbuffers, check the time spent
 * on the whole frame and synt to target framerate.
 */
void EndFrame()
{
    SDL2_Context *ctx = SDL2_GetContext();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(ctx->window);

    Uint64 performance_counter = SDL_GetPerformanceCounter();
    float work_in_seconds = GetSecondsElapsed(ctx->last_performance_counter,
                                              performance_counter);
    float dt = DeltaTime();
    if(work_in_seconds < dt)
    {
        float sec_to_sleep = dt - work_in_seconds;
        unsigned int ms_to_sleep = (unsigned int)(sec_to_sleep * 1000.0f) + 1;
        SDL_Delay(ms_to_sleep);
    }
    ctx->last_performance_counter = SDL_GetPerformanceCounter();
}

uint64_t GetPerformanceCounter()
{
    return SDL_GetPerformanceCounter();
}

/**
 * Get seconds elapsed between to performance counter values.
 */
float GetSecondsElapsed(uint64_t start_counter, uint64_t end_counter)
{
    uint64_t counter_elapsed = end_counter - start_counter;
    float result = (float)counter_elapsed / (float)SDL_GetPerformanceFrequency();
    return result;
}

float DeltaTime()
{
    SDL2_Context *ctx = SDL2_GetContext();
    return ctx->dt;
}

bool IsRunning()
{
    SDL2_Context *ctx = SDL2_GetContext();
    return ctx->is_running;
}

void GetWindowDimensions(int *screen_width, int *screen_height)
{
    SDL2_Context *ctx = SDL2_GetContext();
    SDL_GetWindowSize(ctx->window, screen_width, screen_height);
}

/**
 * Returns currently set SDL2_Context
 * Mostly for internal use!
 */
SDL2_Context *SDL2_GetContext()
{
    ASSERT(g_sdl2_ctx != nullptr);
    return g_sdl2_ctx;
}

/**
 * Initializes SDL, creates a window, sets SDL_GL attributes.
 */
static void SDL2_InitSDL(SDL2_Context *ctx, const char *window_title, int screen_width, int screen_height)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        fprintf(stderr, "error: %s\n", SDL_GetError());
    }

    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Window* window = SDL_CreateWindow(window_title,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          screen_width, screen_height,
                                          window_flags);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetSwapInterval(1); // Enable vsync

    ctx->window = window;
}

/**
 * Initializes OpenGL for SDL, loads GL functions, sets GL attributes
 * used throught the program runtime.
 */
static void SDL2_InitGL(SDL2_Context *ctx)
{
    SDL_GLContext gl_context = SDL_GL_CreateContext(ctx->window);
    SDL_GL_MakeCurrent(ctx->window, gl_context);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    ctx->gl_context = gl_context;
}

/**
 * Initializes DearImGui for SDL/OpenGL, sets config flags.
 */
static void SDL2_InitImGui(SDL2_Context *ctx)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    const char *glsl_version = "#version 420";
    ImGui_ImplSDL2_InitForOpenGL(ctx->window, ctx->gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

static void SDL2_ProcessEvent(const SDL_Event* event)
{
    SDL2_Context *ctx = SDL2_GetContext();

    switch(event->type)
    {
        case SDL_QUIT:
        {
            ctx->is_running = false;
        } break;

        case SDL_WINDOWEVENT:
        {
            if(event->window.event == SDL_WINDOWEVENT_CLOSE &&
               event->window.windowID == SDL_GetWindowID(ctx->window))
            {
                ctx->is_running = false;
            }
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode kc = event->key.keysym.sym;
            bool is_down = (event->key.state == SDL_PRESSED);

            uint16_t mod = event->key.keysym.mod;
            // bool lshift_is_down = (mod & KMOD_LSHIFT);
            // bool ctrl_is_down = (mod & KMOD_CTRL);
            bool alt_is_down = (mod & KMOD_ALT);

            if(event->key.repeat == 0)
            {
                Input::SetByKeycode(kc, is_down);
            }

            if(event->type == SDL_KEYDOWN)
            {
                if((alt_is_down && kc == SDLK_RETURN)
                   || (kc == SDLK_F11))
                {
                    ctx->is_fullscreen = !ctx->is_fullscreen;
                    SDL_WindowFlags flags = (SDL_WindowFlags)0;
                    if(ctx->is_fullscreen)
                        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowFullscreen(ctx->window, flags);
                }

                if(alt_is_down && kc == SDLK_F4)
                {
                    ctx->is_running = false;
                }
            }
        } break;
    }
}

} // namespace OS