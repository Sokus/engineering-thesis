#include <stdio.h>
#include <stdlib.h>

#include "macros.h" // ABS
#include "game/world.h"
#include "user_interface.h"

#include "raylib.h"

enum GameScene
{
    GS_INVALID,
    GS_TITLE_SCREEN,
    GS_GAME
};

enum GameMenu
{
    GM_NONE,
    GM_MAIN_MENU,
    GM_SOLO_PLAY,
    GM_JOIN_MENU,
    GM_HOST_MENU,
    GM_OPTIONS_MENU,
};

struct State
{
    GameScene current_scene = GS_TITLE_SCREEN;
    GameMenu current_menu = GM_NONE;
    bool should_quit = false;

    struct {
        bool world_initialised = false;
    } game;
} state = {};

void DoMainMenu()
{
    UI::Button solo_play_button = UI::Button("Solo Play");
    UI::Button join_game_button = UI::Button("Join Game");
    UI::Button host_game_button = UI::Button("Host Game");
    UI::Button options_button = UI::Button("Options");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&solo_play_button.base); UI::EndRow();
        UI::Add(&join_game_button.base); UI::EndRow();
        UI::Add(&host_game_button.base); UI::EndRow();
        UI::Add(&options_button.base); UI::EndRow();
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if (solo_play_button.IsReleased())
    {
        state.current_scene = GS_GAME;
        state.current_menu = GM_NONE;
    }

    if (join_game_button.IsReleased())
        state.current_menu = GM_JOIN_MENU;

    if (host_game_button.IsReleased())
        state.current_menu = GM_HOST_MENU;

    if (options_button.IsReleased())
        state.current_menu = GM_OPTIONS_MENU;

    if (exit_button.IsReleased())
        state.should_quit = true;

    solo_play_button.Draw();
    join_game_button.Draw();
    host_game_button.Draw();
    options_button.Draw();
    exit_button.Draw();
}

void DoJoinMenu()
{
    static UI::TextField ip_field = UI::TextField(15, "localhost");
    static UI::TextField port_field = UI::TextField(5, "25565");
    UI::Button join_button = UI::Button("Join");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&ip_field.base);
        UI::Add(&port_field.base);
        UI::Add(&join_button.base); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    ip_field.Update();
    port_field.Update();

    if(join_button.IsReleased())
    {
        // nothing done yet
    }

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;



    ip_field.Draw();
    port_field.Draw();
    join_button.Draw();
    close_button.Draw();
}

void DoHostMenu()
{
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;

    close_button.Draw();
}

void DoOptionsMenu()
{
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if(close_button.IsReleased())
        state.current_menu = GM_MAIN_MENU;

    close_button.Draw();
}

void DoTitleScreenScene()
{
    if(state.current_menu == GM_NONE)
        state.current_menu = GM_MAIN_MENU;

    UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
    UI::SetOrigin(0.5f, 0.5f);

    ClearBackground(Color{20, 20, 20});
    switch(state.current_menu)
    {
        case GM_MAIN_MENU: DoMainMenu(); break;
        case GM_JOIN_MENU: DoJoinMenu(); break;
        case GM_HOST_MENU: DoHostMenu(); break;
        case GM_OPTIONS_MENU: DoOptionsMenu(); break;
        default: break;
    }
}

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
        state.current_menu = GM_OPTIONS_MENU;

    if(return_button.IsReleased())
        state.current_menu = GM_NONE;

    if(exit_button.IsReleased())
    {
        state.current_scene = GS_TITLE_SCREEN;
        state.current_menu = GM_NONE;
        state.game.world_initialised = false;
    }

    options_button.Draw();
    return_button.Draw();
    exit_button.Draw();
}

void DoGameScene()
{
    static Game::World world;
    static Game::Entity *player;

    if(!state.game.world_initialised)
    {
        world.Clear();
        player = world.CreatePlayer(100.0f, 100.0f);
        world.CreateTile(100.0f, 250.0f);
        world.CreateTile(164.0f, 250.0f);
        world.CreateTile(228.0f, 250.0f);
        world.CreateTile(292.0f, 250.0f);
        world.CreateTile(292.0f, 314.0f);
        world.CreateTile(356.0f, 314.0f);
        world.CreateTile(420.0f, 314.0f);
        world.CreateInteractive(356.0f, 250.0f);

        state.game.world_initialised = true;
    }

    Game::Input input;
    input.Update();
    player->Control(&input);
    player->onGround = 0;
    float expected_delta_time = 1.0f / (float)GetFPS();
    float delta_time = GetFrameTime() > expected_delta_time ? expected_delta_time : GetFrameTime();
    world.CalculateCollisions(*player, delta_time);
    world.Update(delta_time);

    ClearBackground(Color{25, 30, 40});
    world.Draw();


    if(IsKeyPressed(KEY_ESCAPE))
    {
        if(state.current_menu == GM_NONE)
            state.current_menu = GM_MAIN_MENU;
        else
            state.current_menu = GM_NONE;
    }

    if(state.current_menu != GM_NONE)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});

        UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
        UI::SetOrigin(0.5f, 0.5f);

        switch(state.current_menu)
        {
            case GM_MAIN_MENU: DoPauseMenu(); break;
            case GM_OPTIONS_MENU: DoOptionsMenu(); break;
            default: break;
        }
    }
}

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    Game::LoadTextures();
    UI::Init();

    while(!state.should_quit)
    {
        BeginDrawing();
        switch(state.current_scene)
        {
            case GS_TITLE_SCREEN: DoTitleScreenScene(); break;
            case GS_GAME: DoGameScene(); break;
            default: state.should_quit = true; break;
        }
        EndDrawing();

        if(WindowShouldClose()) state.should_quit = true;
    }

    CloseWindow();

    return 0;
}