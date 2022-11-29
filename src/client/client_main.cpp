#include <stdio.h>
#include <stdlib.h>

#include "macros.h" // ABS
#include "game/world.h"
#include "user_interface.h"

#include "raylib.h"

enum GameScene
{
    INVALID,
    MAIN_MENU,
    GAME
};

struct State
{
    GameScene current_scene = MAIN_MENU;
    bool should_quit = false;

    struct {
        bool world_initialised = false;
    } game;

    struct {
        Font font;
        UI::Style style;
    } ui;
} state = {};


void DoMainMenu()
{
    const float font_size = 40;

    static UI::Layout layout;
    layout.position = Vector2{GetScreenWidth() / 2.0f, 100.0f};
    layout.origin = Vector2{0.5f, 0.0f};
    layout.Clear();

    UI::Button start_button = UI::Button(&state.ui.style, &state.ui.font, "Start", font_size);
    UI::Button exit_button = UI::Button(&state.ui.style, &state.ui.font, "Exit", font_size);
    UI::Button join_button = UI::Button(&state.ui.style, &state.ui.font, "Join", font_size);
    static UI::TextField ip_field = UI::TextField(&state.ui.style, &state.ui.font, 15, "localhost", font_size);
    static UI::TextField port_field = UI::TextField(&state.ui.style, &state.ui.font, 5, "25565", font_size);

    layout.AddElement(&start_button.base);
    layout.EndRow();
    layout.AddElement(&ip_field.base);
    layout.AddElement(&port_field.base);
    layout.AddElement(&join_button.base);
    layout.EndRow();
    layout.AddElement(&exit_button.base);
    layout.EndRow();

    layout.EndColumn();

    if (start_button.IsReleased())
        state.current_scene = GAME;

    ip_field.Update();
    port_field.Update();

    if (join_button.IsReleased())
    {
        // nothing done yet
    }

    if (exit_button.IsReleased())
        state.should_quit = true;

    BeginDrawing();
        ClearBackground(Color{20, 20, 20});

        start_button.Draw();
        join_button.Draw();
        exit_button.Draw();

        ip_field.Draw();
        port_field.Draw();
    EndDrawing();
}

void DoGame()
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
        state.current_scene = MAIN_MENU;
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

    BeginDrawing();
        ClearBackground(Color{25, 30, 40});
        world.Draw();
    EndDrawing();
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
    state.ui.style.spacing = {10.0f, 10.0f};

    while(!state.should_quit)
    {
        switch(state.current_scene)
        {
            case MAIN_MENU: DoMainMenu(); break;
            case GAME: DoGame(); break;
            default: state.should_quit = true; break;
        }

        if(WindowShouldClose()) state.should_quit = true;
    }

    CloseWindow();

    return 0;
}