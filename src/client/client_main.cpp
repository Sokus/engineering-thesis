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

#include "filesystem.h"
#include "platform/input.h"
#include "platform/platform.h"

#include "graphics/opengl3_renderer.h"
#include "graphics/opengl3_draw_queue.h"
#include "graphics/opengl3_debug.h"

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

    Renderer renderer;
    DrawQueue draw_queue;

    OS::GetWindowDimensions(&screen_width, &screen_height);

    while(OS::IsRunning())
    {
        OS::BeginFrame();
        OS::GetWindowDimensions(&screen_width, &screen_height);

        player.control();
        player.update();

        draw_queue.Clear();
        draw_queue.SetViewportDimensions({screen_width, screen_height});
        draw_queue.SetViewMatrix(glm::mat4(1.0f));
        draw_queue.SetProjectionMatrix(CreateProjectionMatrix(screen_width, screen_height, 4.0f));
        draw_queue.SetBackgroundColor({0.1f, 0.15f, 0.2f});
        draw_queue.SetAmbientLightColor({0.1f, 0.15f, 0.2f});

        // Render a light source
        draw_queue.AddLight({0,0}, {2, 2, 1.5f}, 200);

        // Render the player
        int playerTileIdx;
        if(ABS(player.velocity.x) <= 0.001)
            playerTileIdx = (player.lifetime / 10) % 4;
        else
            playerTileIdx = (player.lifetime / 10) % 4 + 8;
        draw_queue.AddEntity(player.position, texture, playerTileIdx, player.facing == Facing::LEFT);

        // Render a shockwave
        Shockwave shockwave;
        float progress =  (SDL_GetTicks() % 500) / 500.0f;
        shockwave.center = glm::vec2(50, 25);
        shockwave.radius = progress * 100;
        shockwave.scale = 20;
        shockwave.strength = 20;
        draw_queue.AddShockwave(shockwave);


        renderer.render(draw_queue);
        OS::EndFrame();
    }

    OS::DestroyContext();

    return 0;
}