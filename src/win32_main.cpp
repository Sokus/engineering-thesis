#include "helpers.h"
#include "platform.h"
#include "pi_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "platform.cpp"
#include "sdl2_platform.cpp"
#include "opengl3_platform.cpp"

Texture LoadTextureAtlas(char *path, int tile_width, int tile_height, int channels)
{
    Texture result = {0};
    
    int width, height, source_channels;
    unsigned char *data = stbi_load(path, &width, &height, &source_channels, channels);
    
    if(data)
    {
        int tiles_x = width / tile_width;
        int tiles_y = height / tile_height;
        int tile_count = tiles_x * tiles_y;
        ASSERT(tile_width > 0 && tile_height > 0 && tile_count > 0);
        
        if(width % tile_width != 0 || height % tile_height != 0)
        {
            fprintf(stderr,
                    "WARNING: Texture size not a "
                    "multiple of tile width/height! (%s)",
                    path);
        }
        
            GLint format = (channels == 3 ? GL_RGB :
                            channels == 4 ? GL_RGBA : GL_RGBA);
                    
            if(format == 0)
            {
            fprintf(stderr, "ERROR: Channels count (%d) not supported! (%s)",
                    channels, path);
            stbi_image_free(data);
            return result;
        }
        
        GLuint gl_texture_id;
        glGenTextures(1, &gl_texture_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, gl_texture_id);
        
        float texture_border_color[] = { 1.0f, 0.0f, 1.0f, 0.0f };
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, texture_border_color);
        
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tile_width, tile_height,
                     tile_count, 0, (GLenum)format, GL_UNSIGNED_BYTE, 0);
        
        unsigned char *tile_buffer = (unsigned char *)malloc(tile_width * tile_height * channels);
        int tile_w_stride = channels * tile_width;
        int row_stride = tile_w_stride * tiles_x;
        for(int tile_y_idx = 0; tile_y_idx < tiles_y; ++tile_y_idx)
        {
            for(int tile_x_idx = 0; tile_x_idx < tiles_x; ++tile_x_idx)
            {
                int offset = tile_y_idx * row_stride * tile_height + tile_x_idx * tile_w_stride;
                unsigned char *tile_corner_ptr = data + offset;
                
                for(int tile_pixel_y = 0; tile_pixel_y < tile_height; ++tile_pixel_y)
                {
                    unsigned char *src = tile_corner_ptr + tile_pixel_y * row_stride;
                    int inverse_tile_pixeL_y = (tile_height - tile_pixel_y - 1);
                    unsigned char *dst = tile_buffer + inverse_tile_pixeL_y * tile_w_stride;
                    MEMORY_COPY(dst, src, (unsigned int)(tile_w_stride));
                }
                
                int layer_idx = tile_y_idx * tiles_x + tile_x_idx;
                
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer_idx,
                                tile_width, tile_height, 1, (GLenum)format,
                                GL_UNSIGNED_BYTE, tile_buffer);
            }
        }
        free(tile_buffer);
        stbi_image_free(data);
        
        result.is_valid = true;
        result.id = gl_texture_id;
        result.width = width;
        result.height = height;
        result.channels = channels;
        result.tile_width = tile_width;
        result.tile_height = tile_height;
        result.tile_count = tile_count;
    }
    else
    {
        fprintf(stderr, "ERROR: Could not load texture: %s\n", path);
    }
    
    return result;
}

void FreeTexture(Texture *texture)
{
    glDeleteTextures(1, &texture->id);
    texture->is_valid = false;
}

void Win32_FreeFileMemory(EntireFile *file)
{
    if(file->data)
        VirtualFree(file->data, 0, MEM_RELEASE);
    file->data = 0;
    file->size = 0;
}

EntireFile Win32_ReadEntireFile(char *filename, bool null_terminate)
{
    EntireFile result = {0};
    
    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,
                                     0, OPEN_EXISTING, 0, 0);
    
    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            unsigned int size_to_read = SafeTruncateU64(file_size.QuadPart);
            unsigned int total_size = size_to_read;
            if(null_terminate) total_size++;
            
            void *data = VirtualAlloc(0, total_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(data)
            {
                DWORD bytes_read;
                if(ReadFile(file_handle, data, size_to_read, &bytes_read, 0) && (size_to_read == bytes_read))
                {
                    result.data = data;
                    result.size = total_size;
                }
                else
                {
                    VirtualFree(data, 0, MEM_RELEASE);
                }
            }
            else
            {
                fprintf(stderr, "ERROR: Could not allocate memory for the file %s.\n", filename);
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Could not get the size of the file %s.\n", filename);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    }
    
    return result;
}

void RenderEntity(EntityProgram *entity_program, Texture *texture, float pos_x, float pos_y, int layer, bool flip)
{
    glDisable(GL_DEPTH_TEST);
    
    float scale_x = (float)texture->tile_width;
    float scale_y = (float)texture->tile_height;
    mat4 scale = Scale(Mat4d(1.0f), scale_x, scale_y, 1.0f);
    mat4 model_to_world = Translate(scale, pos_x, pos_y, 0.0f);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);
    glBindVertexArray(entity_program->vao_handle);
    glUseProgram(entity_program->program_handle);
    
    SetMat4Uniform(entity_program->program_handle, "u_model_to_world", &model_to_world);
    SetIntUniform(entity_program->program_handle, "u_layer", layer);
    SetBoolUniform(entity_program->program_handle, "u_flip", flip);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void EntityProgramUpdateView(EntityProgram *entity_program, int screen_width, int screen_height, float scale)
{
    mat4 scale_matrix = Scale(Mat4d(1.0f), scale, scale, 1.0f);
    
    #if 0
    mat4 orthographic_projection = Orthographic(0.0f, (float)screen_width,
                                                0.0f, (float)screen_height,
                                                -1.0f, 1.0f);
#else
    float half_screen_width = (float)screen_width / 2.0f;
    float half_screen_height = (float)screen_height / 2.0f;
    mat4 orthographic_projection = Orthographic(-half_screen_width, half_screen_width,
                                                -half_screen_height, half_screen_height,
                                                -1.0f, 1.0f);
#endif
    
    mat4 camera_to_clip = MultiplyMat4(orthographic_projection, scale_matrix);
    
    glUseProgram(entity_program->program_handle);
    SetMat4Uniform(entity_program->program_handle, "u_camera_to_clip", &camera_to_clip);
    glUseProgram(0);
}

enum class Facing {LEFT, RIGHT};

struct Player
{
    vec2 position = Vec2(0.0f, 0.0f);
    vec2 velocity = Vec2(0.0f, 0.0f);
    int lifetime = 0;
    Facing facing = Facing::RIGHT;
    static const vec2 max_velocity;

    void update()
    {
        position = AddVec2(position, velocity);

        if(velocity.x < 0) facing = Facing::LEFT;
        if(velocity.x > 0) facing = Facing::RIGHT;
        
        ++lifetime;
    }

    void control(const Input &input) 
    {
        vec2 move_dir = Vec2(0.0f, 0.0f);
        if(input.keys_down[SDLK_a]) move_dir.x -= 1;
        if(input.keys_down[SDLK_d]) move_dir.x += 1;

        velocity = MultiplyVec2(move_dir, max_velocity);
    }
};

const vec2 Player::max_velocity = Vec2(1.0f, 5.0f);

int main(int, char**)
{
    bool is_running = false;
    float target_fps = 60.0f;
    float dt = 1.0f / target_fps;
    
    char *window_title = "Nie patrz mi się na tytuł";
    int screen_width = 960;
        int screen_height = 640;
            
        SDL2_Context sdl2_context = SDL2_CreateContext(window_title, screen_width, screen_height);
            
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
            
        const char *glsl_version = "#version 420";
        ImGui_ImplSDL2_InitForOpenGL(sdl2_context.window, sdl2_context.gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
            
        Uint64 last_counter = SDL_GetPerformanceCounter();
        is_running = true;
            
        Input input = CreateInput();
        bool fullscreen = false;
            
        Player player;
        vec2 camera_p = Vec2(0.0f, 0.0f);
            
        EntityProgram entity_program;
        {
            EntireFile vertex_shader_file = Win32_ReadEntireFile("./res/shaders/standard.vs", true);
            EntireFile fragment_shader_file = Win32_ReadEntireFile("./res/shaders/standard.fs", true);
        entity_program = CreateEntityProgram((const char *)vertex_shader_file.data,
                                             (const char *)fragment_shader_file.data);
            Win32_FreeFileMemory(&vertex_shader_file);
            Win32_FreeFileMemory(&fragment_shader_file);
        }
            
        Texture texture = LoadTextureAtlas("./res/character.png", 16, 24, 4);
            
        while(is_running)
        {
            SDL_GetWindowSize(sdl2_context.window, &screen_width, &screen_height);
            glViewport(0, 0, screen_width, screen_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                SDL2_ProcessEvent(&event, sdl2_context.window, &input, &is_running, &fullscreen, dt);
            }
                    
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
        
        // GAME CODE BEGINS HERE
        
        UpdateInput(&input, dt);

        player.control(input);
        player.update();
        
        EntityProgramUpdateView(&entity_program, screen_width, screen_height, 4.0f);

        int playerTileIdx;
        if(ABS(player.velocity.x) <= 0.001)
            // Player isn't moving
            playerTileIdx = (player.lifetime / 10) % 4;
        else
            // Player is moving
            playerTileIdx = (player.lifetime / 10) % 4 + 8;
        
        RenderEntity(&entity_program, &texture, player.position.x, player.position.y, playerTileIdx, player.facing == Facing::LEFT);
        //RenderEntity(&entity_program, &texture, 12.0f, 1.0f, 0, false);
        
        // GAME CODE ENDS HERE
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(sdl2_context.window);
        
        Uint64 work_counter = SDL_GetPerformanceCounter();
        float work_in_seconds = SDL2_GetSecondsElapsed(last_counter, work_counter);
        if(work_in_seconds < dt)
        {
            float sec_to_sleep = dt - work_in_seconds;
            unsigned int ms_to_sleep = (unsigned int)(sec_to_sleep * 1000.0f) + 1;
            SDL_Delay(ms_to_sleep);
        }
        last_counter = SDL_GetPerformanceCounter();
        }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL2_DestroyContext(&sdl2_context);
    
    return 0;
}