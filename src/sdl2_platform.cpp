#define DEFAULT_KEY_UP SDLK_w
#define DEFAULT_KEY_LEFT SDLK_a
#define DEFAULT_KEY_DOWN SDLK_s
#define DEFAULT_KEY_RIGHT SDLK_d

#define SDL2_KEYCODE_OFFSET (-0x40000039 + 0x80)
#define SDL2_KEYCODE_COUNT 354

struct SDL2_Context
{
    bool is_valid;
    SDL_Window *window;
    SDL_GLContext gl_context;
};

SDL2_Context SDL2_CreateContext(const char* window_title,
                                int screen_width,
                                int screen_height)
{
    SDL2_Context result = {0};
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return result;
    }
    
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL
                                                     | SDL_WINDOW_RESIZABLE
                                                     | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(window_title,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          screen_width, screen_height,
                                          window_flags);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    //SDL_GL_SetSwapInterval(1); // Enable vsync
    
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    
    result.is_valid = true;
    result.window = window;
    result.gl_context = gl_context;
    return result;
}

void SDL2_DestroyContext(SDL2_Context *sdl2_context)
{
    SDL_GL_DeleteContext(sdl2_context->gl_context);
    SDL_DestroyWindow(sdl2_context->window);
    SDL_Quit();
    
}

// SDL Keycodes:
// can be represented:    from 0x00 to 0x7F, 0x80 total (128)             
// cannot be represented: 0x40000039 - 0x4000011A, 0xE2 total (226)
// 128 + 226 = 354
unsigned int SDL2_KeycodeToIndex(SDL_Keycode keycode)
{
    unsigned int result = 0;
    result = (keycode <= 0x7F) ? keycode : (keycode + SDL2_KEYCODE_OFFSET);
    return result;
}

void SDL2_ProcessEvent(SDL_Event *event, SDL_Window *window,
                       Input *input, bool *is_running, bool *fullscreen, float dt)
{
    switch(event->type)
    {
        case SDL_QUIT:
        {
            *is_running = false;
        } break;
        
        case SDL_WINDOWEVENT:
        {
            if(event->window.event == SDL_WINDOWEVENT_CLOSE
               && event->window.windowID == SDL_GetWindowID(window))
            {
                *is_running = false;
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
                unsigned int key_index = SDL2_KeycodeToIndex(kc);
                input->keys_down[key_index] = is_down;
            }
            
            if(event->type == SDL_KEYDOWN)
            {
                if((alt_is_down && kc == SDLK_RETURN)
                   || (kc == SDLK_F11))
                {
                    *fullscreen = !(*fullscreen);
                    SDL_WindowFlags flags = (SDL_WindowFlags)0;
                    if(*fullscreen)
                        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowFullscreen(window, flags);
                }
                
                if(alt_is_down && kc == SDLK_F4)
                {
                    *is_running = false;
                }
            }
        } break;
    }
}

float SDL2_GetSecondsElapsed(Uint64 start_counter, Uint64 end_counter)
{
    Uint64 counter_elapsed = end_counter - start_counter;
    float result = (float)counter_elapsed / (float)SDL_GetPerformanceFrequency();
    return result;
}
