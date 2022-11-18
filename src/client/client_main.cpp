#include <stdio.h>
#include <stdlib.h>


#include "config.h"
#include "macros.h" // ABS
#include "game/world.h"

#include "raylib.h"

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);

    Game::Input input;
    Game::World world;

    Game::LoadTextures();

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);
    world.CreateTile(250.0f,150.0f);

    while(!WindowShouldClose())
    {
        input.Update();
        player->Control(&input);
        world.CalculateCollisions(*player);
        world.Update(GetFrameTime());

        BeginDrawing();
            ClearBackground(Color{25, 30, 40});
            world.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}