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

    struct {
        Font font;
        UI::Style style;
        const float menu_font_size = 30;
        UI::Layout layout;
    } ui;
} state = {};

void DoMainMenu()
{
    state.ui.layout.Clear();

    UI::Button solo_play_button = UI::Button(&state.ui.style, &state.ui.font, "Solo Play", state.ui.menu_font_size);
    UI::Button join_game_button = UI::Button(&state.ui.style, &state.ui.font, "Join Game", state.ui.menu_font_size);
    UI::Button host_game_button = UI::Button(&state.ui.style, &state.ui.font, "Host Game", state.ui.menu_font_size);
    UI::Button options_button = UI::Button(&state.ui.style, &state.ui.font, "Options", state.ui.menu_font_size);
    UI::Button exit_button = UI::Button(&state.ui.style, &state.ui.font, "Exit", state.ui.menu_font_size);
    state.ui.layout.AddElement(&solo_play_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.AddElement(&join_game_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.AddElement(&host_game_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.AddElement(&options_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.AddElement(&exit_button.base);
    state.ui.layout.EndRow();

    state.ui.layout.EndColumn();

    if (solo_play_button.IsReleased())
    {
        state.current_scene = GS_GAME;
        state.current_menu = GM_NONE;
    }

    if (join_game_button.IsReleased())
    {
        state.current_menu = GM_JOIN_MENU;
    }

    if (host_game_button.IsReleased())
    {
        state.current_menu = GM_HOST_MENU;
    }

    if (options_button.IsReleased())
    {
        // nothing done yet
    }

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
    state.ui.layout.Clear();

    static UI::TextField ip_field = UI::TextField(&state.ui.style, &state.ui.font, 15, "localhost", state.ui.menu_font_size);
    static UI::TextField port_field = UI::TextField(&state.ui.style, &state.ui.font, 5, "25565", state.ui.menu_font_size);
    UI::Button join_button = UI::Button(&state.ui.style, &state.ui.font, "Join", state.ui.menu_font_size);
    UI::Button close_button = UI::Button(&state.ui.style, &state.ui.font, "Close", state.ui.menu_font_size);

    state.ui.layout.AddElement(&ip_field.base);
    state.ui.layout.AddElement(&port_field.base);
    state.ui.layout.AddElement(&join_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.AddElement(&close_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.EndColumn();

    ip_field.Update();
    port_field.Update();

    if(join_button.IsReleased())
    {
        // nothing done yet
    }

    if(close_button.IsReleased())
    {
        state.current_menu = GM_MAIN_MENU;
    }

    ip_field.Draw();
    port_field.Draw();
    join_button.Draw();
    close_button.Draw();
}

void DoHostMenu()
{
    state.ui.layout.Clear();

    UI::Button close_button = UI::Button(&state.ui.style, &state.ui.font, "Close", state.ui.menu_font_size);

    state.ui.layout.AddElement(&close_button.base);
    state.ui.layout.EndRow();
    state.ui.layout.EndColumn();

    if(close_button.IsReleased())
    {
        state.current_menu = GM_MAIN_MENU;
    }

    close_button.Draw();
}

void DoTitleScreenScene()
{
    if(state.current_menu == GM_NONE)
        state.current_menu = GM_MAIN_MENU;

    ClearBackground(Color{20, 20, 20});
    switch(state.current_menu)
    {
        case GM_MAIN_MENU: DoMainMenu(); break;
        case GM_JOIN_MENU: DoJoinMenu(); break;
        case GM_HOST_MENU: DoHostMenu(); break;
        default: break;
    }
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

    if (IsKeyPressed(KEY_ESCAPE))
    {
        state.current_scene = GS_TITLE_SCREEN;
        state.game.world_initialised = false;
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
}

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    Game::LoadTextures();

    state.ui.font = LoadFontEx(RESOURCE_PATH "/LiberationMono-Regular.ttf", 96, 0, 0);
    SetTextureFilter(state.ui.font.texture, TEXTURE_FILTER_BILINEAR);

    state.ui.style.fill_default = LIGHTGRAY;
    state.ui.style.fill_active = WHITE;
    state.ui.style.text_default = DARKGRAY;
    state.ui.style.text_suggestion = GRAY;
    state.ui.style.outline_active = RED;
    state.ui.style.padding = {10.0f, 10.0f};
    state.ui.style.spacing = {4.0f, 4.0f};

    state.ui.layout.position = Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    state.ui.layout.origin = Vector2{0.5f, 0.5f};

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