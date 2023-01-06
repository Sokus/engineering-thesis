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

AppData app_state = {};
GameData game_data = {};

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
        app_state.current_menu = GAME_MENU_OPTIONS;

    if(return_button.IsReleased())
        app_state.current_menu = GAME_MENU_NONE;

    if(exit_button.IsReleased())
    {
        app_state.current_scene = GAME_SCENE_TITLE_SCREEN;
        app_state.current_menu = GAME_MENU_NONE;
        game_data.world.initialised = false;
    }

    options_button.Draw();
    return_button.Draw();
    exit_button.Draw();
}

void DoGameScene()
{
    static Game::Entity *player;
    if(!game_data.world.initialised)
    {
        game_data.world.Clear();
        game_data.world.SetLevel(app_state.level_selected);
        Texture2D character = LoadTexture(RESOURCE_PATH "/character.png");
        Vector2 spawnpoint = app_state.level_selected->spawnpoint;
        player = game_data.world.CreatePlayer(spawnpoint.x, spawnpoint.y, character, app_state.player_type_selected).entity;

        game_data.world.initialised = true;
    }
    if (game_data.world.level.finished) {
        game_data.world.Clear();
        app_state.current_menu = GAME_MENU_WON;
    }

    float expected_delta_time = 1.0f / (float)GetFPS();
    float delta_time = GetFrameTime() > expected_delta_time ? expected_delta_time : GetFrameTime();

    Game::Input input = Game::GetInput();
    game_data.world.Update(&input, delta_time);

    Camera2D camera = {};
    camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 3.0f;
    camera.target.x = player->position.x;
    camera.target.y = player->position.y;
    camera.zoom = 4.0f;

    BeginMode2D(camera);
    ClearBackground(Color{25, 30, 40});
    game_data.world.Draw();
    EndMode2D();

    if(IsKeyPressed(KEY_ESCAPE))
    {
        if(app_state.current_menu == GAME_MENU_NONE)
            app_state.current_menu = GAME_MENU_MAIN;
        else
            app_state.current_menu = GAME_MENU_NONE;
    }

    if(app_state.current_menu != GAME_MENU_NONE)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});

        UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
        UI::SetOrigin(0.5f, 0.5f);

        switch(app_state.current_menu)
        {
            case GAME_MENU_MAIN: DoPauseMenu(); break;
            case GAME_MENU_OPTIONS: DoOptionsMenu(); break;
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

    app_state.last_scene = GAME_SCENE_INVALID;
    app_state.current_scene = GAME_SCENE_TITLE_SCREEN;
    app_state.last_menu = GAME_MENU_NONE;
    app_state.current_menu = GAME_MENU_MAIN;

    while(!app_state.should_quit)
    {
        app_state.scene_changed = (app_state.current_scene != app_state.last_scene);
        app_state.last_scene = app_state.current_scene;
        app_state.menu_changed = (app_state.current_menu != app_state.last_menu);
        app_state.last_menu = app_state.current_menu;

        BeginDrawing();
        switch(app_state.current_scene)
        {
            case GAME_SCENE_TITLE_SCREEN: DoTitleScreenScene(); break;
            case GAME_SCENE_GAME: DoGameScene(); break;
            default: app_state.should_quit = true; break;
        }
        EndDrawing();

        if(WindowShouldClose()) app_state.should_quit = true;
    }

    CloseWindow();

    return 0;
}