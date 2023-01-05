#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h" // ABS
#include "game/world.h"
#include "user_interface.h"
#include "system/pi_time.h"
#include "game/input.h"
#include "client.h"
#include "title_screen.h"

#include "raylib.h"

GameState game_state = {};

void DoPauseMenu()
{
    UI::Button options_button = UI::Button("Options");
    UI::Button return_button = UI::Button("Return");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&options_button.base); UI::EndRow();
        UI::Add(&return_button.base);
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if(options_button.IsReleased())
        game_state.current_menu = GM_OPTIONS_MENU;

    if(return_button.IsReleased())
        game_state.current_menu = GM_NONE;

    if(exit_button.IsReleased())
    {
        game_state.current_scene = GS_TITLE_SCREEN;
        game_state.current_menu = GM_NONE;
    }

    options_button.Draw();
    return_button.Draw();
    exit_button.Draw();
}

void DoGameScene()
{
    static Game::World world;

    static Game::Entity *player;
    if(!world.initialised)
    {
        world.Clear();
        world.SetLevel(Game::ActualLevel);
        Texture2D character = LoadTexture(RESOURCE_PATH "/character.png");
        player = world.CreatePlayer(Game::ActualLevel.spawnpoint.x, Game::ActualLevel.spawnpoint.y,character,Game::ActualPlayer).entity;

        world.initialised = true;
    }

    float expected_delta_time = 1.0f / (float)GetFPS();
    float delta_time = GetFrameTime() > expected_delta_time ? expected_delta_time : GetFrameTime();

    Game::Input input = Game::GetInput();
    world.Update(&input, delta_time);

    Camera2D camera = {};
    camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 3.0f;
    camera.target.x = player->position.x;
    camera.target.y = player->position.y;
    camera.zoom = 4.0f;

    BeginMode2D(camera);
    ClearBackground(Color{25, 30, 40});
    world.Draw();
    EndMode2D();

    if(IsKeyPressed(KEY_ESCAPE))
    {
        if(game_state.current_menu == GM_NONE)
            game_state.current_menu = GM_MAIN_MENU;
        else
            game_state.current_menu = GM_NONE;
    }

    if(game_state.current_menu != GM_NONE)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});

        UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
        UI::SetOrigin(0.5f, 0.5f);

        switch(game_state.current_menu)
        {
            case GM_MAIN_MENU: DoPauseMenu(); break;
            case GM_OPTIONS_MENU: DoOptionsMenu(); break;
            default: break;
        }
    }
}

int main(int, char**)
{
    SetTraceLogLevel(LOG_INFO);

    InitWindow(960, 540, "PI");
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    Game::InitGameContent();
    UI::Init();

    while(!game_state.should_quit)
    {
        game_state.scene_changed = (game_state.current_scene != game_state.last_scene);
        game_state.last_scene = game_state.current_scene;
        game_state.menu_changed = (game_state.current_menu != game_state.last_menu);
        game_state.last_menu = game_state.current_menu;

        BeginDrawing();
        switch(game_state.current_scene)
        {
            case GS_TITLE_SCREEN: DoTitleScreenScene(); break;
            case GS_GAME: DoGameScene(); break;
            default: game_state.should_quit = true; break;
        }
        EndDrawing();

        if(WindowShouldClose()) game_state.should_quit = true;
    }

    CloseWindow();

    return 0;
}