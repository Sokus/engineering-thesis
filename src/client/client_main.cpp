#include <stdio.h>
#include <stdlib.h>


#include "config.h"
#include "macros.h" // ABS
#include "game/world.h"

#include "raylib.h"

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60); //TODO parametrize deltaTime

    Game::Input input;
    Game::World world;

    Game::LoadTextures();

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);
    world.CreateTile(100.0f,250.0f);
    world.CreateTile(164.0f, 250.0f);
    world.CreateTile(228.0f, 250.0f);
    world.CreateTile(292.0f, 250.0f);
    world.CreateTile(292.0f, 314.0f);
    world.CreateTile(356.0f, 314.0f);
    world.CreateTile(420.0f, 314.0f);
    world.CreateInteractive(356.0f, 250.0f);

    while(!WindowShouldClose())
    {
        input.Update();
        player->Control(&input);
        player->onGround = 0;
        float deltaTime = GetFrameTime() > 0.02f ? 0.02f : GetFrameTime(); //will broke if < 60 FPS
        world.CalculateCollisions(*player,deltaTime);
        world.Update(deltaTime);

        BeginDrawing();
            ClearBackground(Color{25, 30, 40});
            world.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}