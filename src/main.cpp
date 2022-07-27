// RELATED HEADERS
// ...

// C HEADERS
#include <stdio.h>
#include <stdlib.h>

// C++ HEADERS
// ...

// LIBRARY HEADERS
#include "glm/glm.hpp"

#include "imgui.h"

// PROJECT HEADERS
#include "config.h"
#include "base/base.h"

#include "os/filesystem.h"
#include "os/input.h"
#include "os/sdl2/sdl2_platform.h"

#include "gfx/opengl3_utils.h"
#include "gfx/opengl3_texture.h"
#include "gfx/opengl3_entity.h"
#include "gfx/opengl3_shockwave.h"
#include "gfx/opengl3_lights.h"
#include "gfx/opengl3_postprocessing.h"
#include "gfx/opengl3_debug.h"

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
        if(OS::IsKeyDown(OS::Keybind::Left)) move_dir.x -= 1.0f;
        if(OS::IsKeyDown(OS::Keybind::Right)) move_dir.x += 1.0f;

        velocity.x = move_speed * move_dir.x;
    }
};

const float Player::move_speed = 4.0f;

int main(int, char**)
{
    OS::CreateContext("Nie patrz mi sie na tytul", 960, 540);
    #ifndef NDEBUG
        EnableThrowOnOpenGLErrors();
    #endif

    int screen_width, screen_height;
    Player player;
    glm::vec2 camera_p = glm::vec2(0.0f, 0.0f);

    Texture texture;
    texture.Load(RESOURCE_PATH "/character.png", 16, 24, 4);

    EntityRenderer entityRenderer;
    entityRenderer.init();

    LightRenderer lightRenderer;
    lightRenderer.init();

    ShockwaveRenderer shockwaveRenderer;
    shockwaveRenderer.init();

    OS::GetWindowDimensions(&screen_width, &screen_height);
    Framebuffers framebuffers;
    framebuffers.init(screen_width, screen_height);


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
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.light_fbo);

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
        entityRenderer.update_view(projection_matrix);

        GLuint TEMPORARY_entity_shader_program = entityRenderer.TEMPORARY_get_shader_program();
        glUseProgram(TEMPORARY_entity_shader_program);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffers.light_map);
        SetIntUniform(TEMPORARY_entity_shader_program, "light_map", 1);

        int playerTileIdx;
        if(ABS(player.velocity.x) <= 0.001)
            playerTileIdx = (player.lifetime / 10) % 4;
        else
            playerTileIdx = (player.lifetime / 10) % 4 + 8;

        entityRenderer.render(player.position.x, player.position.y,
                              texture, playerTileIdx,
                              player.facing == Facing::LEFT);

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

        Shockwave shockwave;
        float progress =  (SDL_GetTicks() % 500) / 500.0f;
        shockwave.center = glm::vec2(50, 25);
        shockwave.radius = progress * 100;
        shockwave.scale = 20;
        shockwave.strength = 20;
        shockwaveRenderer.render(projection_matrix, framebuffers.draw_texture, &shockwave, 1);

        // Blit (copy) postprocessing results into default framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.draw_fbo);
        glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // render some shockwaves

        OS::EndFrame();
    }

    OS::DestroyContext();

    return 0;
}