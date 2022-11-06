#include <stdio.h>
#include <stdlib.h>

#include "glm/glm.hpp"

#include "config.h"
#include "macros.h" // ABS
#include "game/game.h"
#include "game/input.h"

#include "raylib.h"

Texture2D character;

void Game::Entity::Draw()
{
    int scale = 4;
    int sprite_w = 16;
    int sprite_h = 24;
    Rectangle source = Rectangle{0.0f, 0.0f, 0.0f, (float)sprite_h};
    source.width = (float)(facing >= 0 ? sprite_w : -sprite_w);

    if(type == PLAYER)
    {
        bool moving = (ABSF(velocity.x) >= 0.01f || ABSF(velocity.y) >= 0.01f);
        source.y = (float)((2*variant + moving) * sprite_h);
        source.x = (float)(animation_frame % 4 * sprite_w);
        Rectangle dest = {
            position.x,
            position.y,
            (float)(scale * sprite_w),
            (float)(scale * sprite_h)
        };

        DrawTexturePro(character, source, dest, Vector2{0.0f, 0.0f}, 0.0f, Color{255, 255, 255, 255});
    }
    else if(type == ENEMY)
    {

    }
}

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);

    Game::Input input;
    Game::World world;

    character = LoadTexture(RESOURCE_PATH "/character.png");

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);
    player->variant = Game::PlayerVariant::PLAYER_BLUE;

    while(!WindowShouldClose())
    {
        input.Update();
        player->Control(&input);
        world.Update(GetFrameTime());

        BeginDrawing();
            ClearBackground(Color{25, 30, 40});
            world.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}