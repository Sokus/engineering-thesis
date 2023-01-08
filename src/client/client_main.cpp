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
#include "game/entity.h"

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

void DoGameScene(float dt)
{
    static Game::EntityReference player_reference;

    if(!game_data.world.initialised)
    {
        game_data.world.Clear();
        Game::InitLevel(&game_data.world, app_state.level_type_selected);
        player_reference = game_data.world.CreatePlayer(1, game_data.world.spawnpoint.x, game_data.world.spawnpoint.y, app_state.player_type_selected).reference;

        game_data.world.initialised = true;
    }

    Game::Input inputs[2] = {};
    inputs[1] = Game::GetInput();

    game_data.world.Update(inputs, ARRAY_SIZE(inputs), dt);

    Camera2D camera = {};
    camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 4.0f;

    if (!game_data.world.EntityReferenceIsValid(player_reference))
        player_reference = game_data.world.GetOwnedEntityReference(Game::ENTITY_TYPE_PLAYER, 1, 0);

    Game::Entity *player = game_data.world.GetEntityByReference(player_reference);
    if (player)
    {
        camera.target = player->position;
    }

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

    if (game_data.world.finished)
    {
        game_data.world.Clear();
        game_data.world.initialised = false;
        game_data.world.finished = false;
        app_state.current_menu = GAME_MENU_WON;
        app_state.current_scene = GAME_SCENE_TITLE_SCREEN;
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
            case GAME_MENU_WON: DoLevelMenu(); break;
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

    InitializeNetwork();
    InitializeTime();
    Game::LoadEntityTextures();
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

        float expected_delta_time = 1.0f / (float)GetFPS();
        float dt = GetFrameTime() > expected_delta_time ? expected_delta_time : GetFrameTime();

        BeginDrawing();
        switch(app_state.current_scene)
        {
            case GAME_SCENE_TITLE_SCREEN: DoTitleScreenScene(); break;
            case GAME_SCENE_GAME: DoGameScene(dt); break;
            default: app_state.should_quit = true; break;
        }
        EndDrawing();

        if(WindowShouldClose()) app_state.should_quit = true;
    }

    CloseWindow();

    return 0;
}