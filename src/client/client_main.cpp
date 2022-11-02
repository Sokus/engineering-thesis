#include <stdio.h>
#include <stdlib.h>

#include "glm/glm.hpp"

#include "config.h"
#include "macros.h" // ABS
#include "filesystem.h"

#include "raylib.h"

enum class Facing {LEFT, RIGHT};

struct Player
{
    static const float move_speed;

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    unsigned int lifetime = 0;
    Facing facing = Facing::RIGHT;

    void Update()
    {
        position += velocity * GetFrameTime();

        if(velocity.x < 0) facing = Facing::LEFT;
        if(velocity.x > 0) facing = Facing::RIGHT;

        ++lifetime;
    }

    void Control()
    {
        glm::vec2 move_dir = glm::vec2(0.0f, 0.0f);
        // Left/right movement
        if(IsKeyDown(KEY_A)) move_dir.x -= 1.0f;
        if(IsKeyDown(KEY_D)) move_dir.x += 1.0f;

        velocity.x = move_speed * move_dir.x;
    }
};

const float Player::move_speed = 100.0f;

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);

    Player player;
    glm::vec2 camera_p = glm::vec2(0.0f, 0.0f);

    Texture2D character = LoadTexture(RESOURCE_PATH "/character.png");

    while(!WindowShouldClose())
    {
        player.Control();
        player.Update();

        BeginDrawing();
            ClearBackground(Color{25, 30, 40});

            int scale = 4;
            int sprite_w = 16;
            int sprite_h = 24;
            Rectangle source = Rectangle{0.0f, 0.0f, 0.0f, (float)sprite_h};
            source.x = (float)(player.lifetime / 10 % 4 * sprite_w);
            source.y = (ABSF(player.velocity.x) <= 0.001f ? 0.0f : (float)sprite_h);
            source.width = (float)(player.facing == Facing::RIGHT ? sprite_w : -sprite_w);
            Rectangle dest = {
                player.position.x,
                player.position.y,
                (float)(scale * sprite_w),
                (float)(scale * sprite_h)
            };

            DrawTexturePro(character, source, dest, Vector2{0.0f, 0.0f}, 0.0f, Color{255, 255, 255, 255});

        EndDrawing();

        // Render the player
        /*
        int player_tile_idx;
        if(ABS(player.velocity.x) <= 0.001)
            player_tile_idx = (player.lifetime / 10) % 4;
        else
            player_tile_idx = (player.lifetime / 10) % 4 + 8;
        draw_queue.AddEntity(player.position, texture, player_tile_idx, player.facing == Facing::LEFT);
        */
    }

    UnloadTexture(character);

    CloseWindow();

    return 0;
}