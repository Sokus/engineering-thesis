// RELATED HEADERS
// ...

// C HEADERS
#include <stdio.h>
#include <stdlib.h>

// C++ HEADERS
// ...

// LIBRARY HEADERS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/exponential.hpp"
#include "glm/ext/scalar_constants.hpp"

#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// PROJECT HEADERS
#include "config.h"
#include "base/base.h"
#include "os/filesystem.h"
#include "os/input.h"
#include "os/sdl2/sdl2_platform.h"

// SOURCES (TO BE DELETED)
#include "opengl3_platform.cpp"
#include "opengl3_lights.cpp"
#include "opengl3_postprocessing.cpp"
#include "opengl3_shockwave.cpp"

typedef struct Texture
{
    bool is_valid;
    unsigned int id;
    int width;
    int height;
    int channels;
    int tile_width;
    int tile_height;
    int tile_count_x;
    int tile_count_y;
    int tile_count;
} Texture;

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
            fprintf(stderr, "ERROR: Channels count (%d) not supported! (%s)", channels, path);
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

void RenderEntity(EntityProgram *entity_program, Texture *texture, float pos_x, float pos_y, int layer, bool flip)
{
    glDisable(GL_DEPTH_TEST);

    float scale_x = (float)texture->tile_width;
    float scale_y = (float)texture->tile_height;
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_x, scale_y, 1.0f));
    glm::mat4 model_to_world = glm::translate(scale, glm::vec3(pos_x, pos_y, 0.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);
    glBindVertexArray(entity_program->vao_handle);
    glUseProgram(entity_program->program_handle);

    SetMat4Uniform(entity_program->program_handle, "u_model_to_world", model_to_world);
    SetIntUniform(entity_program->program_handle, "u_layer", layer);
    SetIntUniform(entity_program->program_handle, "u_textures", 0);
    SetBoolUniform(entity_program->program_handle, "u_flip", flip);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

glm::mat4 CreateProjectionMatrix(int screen_width, int screen_height, float scale)
{
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

    float half_screen_width = (float)screen_width / 2.0f;
    float half_screen_height = (float)screen_height / 2.0f;
    glm::mat4 orthographic_projection = glm::ortho(-half_screen_width, half_screen_width,
                                                -half_screen_height, half_screen_height,
                                                -1.0f, 1.0f);
    glm::mat4 result = orthographic_projection * scale_matrix;
    return result;
}

void EntityProgramUpdateView(EntityProgram *entity_program, const glm::mat4 &camera_to_clip)
{
    glUseProgram(entity_program->program_handle);
    SetMat4Uniform(entity_program->program_handle, "u_camera_to_clip", camera_to_clip);
    glUseProgram(0);
}

enum class Facing {LEFT, RIGHT};

struct Player
{
    static const float move_speed;

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    int lifetime = 0;
    Facing facing = Facing::RIGHT;

    void update()
    {
        position += velocity * OS::DeltaTime();

        if(velocity.x < 0) facing = Facing::LEFT;
        if(velocity.x > 0) facing = Facing::RIGHT;

        ++lifetime;
    }

    void control()
    {
        glm::vec2 move_dir = glm::vec2(0.0f, 0.0f);
        // Left/right movement
        if(OS::IsKeyDown(SDLK_a)) move_dir.x -= 1;
        if(OS::IsKeyDown(SDLK_d)) move_dir.x += 1;

        velocity.x = move_speed * move_dir.x;
    }
};

const float Player::move_speed = 4.0f;

int main(int, char**)
{
    OS::CreateContext("Nie patrz mi sie na tytul", 960, 540);

    int screen_width, screen_height;
    Player player;
    glm::vec2 camera_p = glm::vec2(0.0f, 0.0f);

    EntityProgram entity_program;
    {
        EntireFile vertex_shader_file = ReadEntireFile(RESOURCE_PATH "/shaders/standard.vs", true);
        EntireFile fragment_shader_file = ReadEntireFile(RESOURCE_PATH "/shaders/standard.fs", true);
        entity_program = CreateEntityProgram((const char *)vertex_shader_file.data,
                                             (const char *)fragment_shader_file.data);
        FreeFileMemory(&vertex_shader_file);
        FreeFileMemory(&fragment_shader_file);
    }

    LightRenderer lightRenderer;
    lightRenderer.init();

    ShockwaveRenderer shockwaveRenderer;
    shockwaveRenderer.init();

    OS::GetWindowDimensions(&screen_width, &screen_height);
    Framebuffers framebuffers;
    framebuffers.init(screen_width, screen_height);

    Texture texture = LoadTextureAtlas(RESOURCE_PATH "/character.png", 16, 24, 4);

    while(OS::IsRunning())
    {
        OS::BeginFrame();
        OS::GetWindowDimensions(&screen_width, &screen_height);

        if(screen_width != framebuffers.fbo_width ||
           screen_height != framebuffers.fbo_height)
        {
            framebuffers.dispose();
            framebuffers.init(screen_width, screen_height);
        }

        player.control();
        player.update();

        const glm::mat4 projection_matrix = CreateProjectionMatrix(screen_width, screen_height, 4.0f);

        // Render lights into light map
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.light_map);

        glm::vec3 ambient_light = glm::vec3(0.1f, 0.15f, 0.2f);
        glClearColor(ambient_light.r, ambient_light.g, ambient_light.b, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        Light light;
        light.position = glm::vec2(0,0);
        light.color = glm::vec3(2, 2, 1.5f);
        light.set_range(200);

        glBlendEquation(GL_MAX);
        lightRenderer.render(projection_matrix, &light, 1);

        // Render ingame objects into draw_fbo
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.draw_fbo);

        glm::vec3 background_color = ambient_light;
        glClearColor(background_color.r, background_color.g, background_color.b, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render lights again
        light.color = 0.15f * light.color;
        glBlendEquation(GL_FUNC_ADD);
        lightRenderer.render(projection_matrix, &light, 1);

        // Render the player
        EntityProgramUpdateView(&entity_program, projection_matrix);
        glUseProgram(entity_program.program_handle);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffers.light_map);
        SetIntUniform(entity_program.program_handle, "light_map", 1);

        int playerTileIdx;
        if(ABS(player.velocity.x) <= 0.001)
            playerTileIdx = (player.lifetime / 10) % 4;
        else
            playerTileIdx = (player.lifetime / 10) % 4 + 8;

        RenderEntity(&entity_program, &texture, player.position.x, player.position.y, playerTileIdx, player.facing == Facing::LEFT);
        //RenderEntity(&entity_program, &texture, 12.0f, 1.0f, 0, false);

        // Extract bright fragments from draw_fbo into bloom_fbo
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.bloom_fbo);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(extract_bright_fragments_program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffers.draw_texture);
        SetIntUniform(extract_bright_fragments_program, "tex", 0);

        bind_dummy_vao();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Apply blur to bloom_fbo
        float kernel[15], variance = 5;
        int no_passes = 5;
        init_gaussian_blur_kernel_1d(kernel, ARRAY_SIZE(kernel), variance);
        ping_pong_blur(kernel, ARRAY_SIZE(kernel), no_passes,
                       framebuffers.bloom_fbo, framebuffers.bloom_texture,
                       framebuffers.buf_fbo, framebuffers.buf_texture);

        // Apply bloom effect
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.draw_fbo);
        glUseProgram(identity_program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffers.bloom_texture);
        SetIntUniform(identity_program, "tex", 0);
        bind_dummy_vao();
        glBlendFunc(GL_ONE, GL_ONE);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Blit (copy) postprocessing results into default framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.draw_fbo);
        glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // render some shockwaves
        Shockwave shockwave;
        float progress =  (SDL_GetTicks() % 500) / 500.0f;
        shockwave.center = glm::vec2(50, 25);
        shockwave.radius = progress * 100;
        shockwave.scale = 20;
        shockwave.strength = 20;

        shockwaveRenderer.render(projection_matrix, framebuffers.draw_texture, &shockwave, 1);

        OS::EndFrame();
    }


    return 0;
}