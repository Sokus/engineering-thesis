#include <stdio.h>
#include <stdlib.h>

#include "glm/glm.hpp"

#include "config.h"
#include "macros.h" // ABS
#include "game/game.h"
#include "game/input.h"

#include "raylib.h"

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);

    Game::Input input;
    Game::World world;

    Game::LoadTextures();

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);

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