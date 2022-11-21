#include <stdio.h>
#include <stdlib.h>


#include "config.h"
#include "macros.h" // ABS
#include "game/world.h"
#include "user_interface.h"

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

enum GameWindow
{
    INVALID,
    MAIN_MENU,
    GAME
};

int main(int, char**)
{
    InitWindow(960, 540, "PI");
    SetTargetFPS(60); //TODO parametrize deltaTime
    SetExitKey(KEY_END);

    Game::Input input;
    Game::World world;

    character = LoadTexture(RESOURCE_PATH "/character.png");

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);
    world.CreateTile(100.0f, 250.0f);
    world.CreateTile(164.0f, 250.0f);
    world.CreateTile(228.0f, 250.0f);
    world.CreateTile(292.0f, 250.0f);
    world.CreateTile(292.0f, 314.0f);
    world.CreateTile(356.0f, 314.0f);
    world.CreateTile(420.0f, 314.0f);
    world.CreateInteractive(356.0f, 250.0f);
    player->variant = Game::PlayerVariant::PLAYER_BLUE;

    GameWindow active_window = MAIN_MENU;

    Font font = LoadFontEx(RESOURCE_PATH "/LiberationMono-Regular.ttf", 96, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    bool exit_window = false;
    while(!exit_window)
    {
        if(WindowShouldClose()) exit_window = true;

        if(active_window == MAIN_MENU)
        {
            const float font_size = 40;

            UI::Style style = {};
            style.fill_default = LIGHTGRAY;
            style.fill_active = WHITE;
            style.text_default = DARKGRAY;
            style.text_suggestion = GRAY;
            style.outline_active = RED;
            style.padding = { 10.0f, 10.0f };
            style.spacing = { 10.0f, 10.0f };

            static UI::Layout layout;
            layout.position = Vector2{ GetScreenWidth()/2.0f, 100.0f };
            layout.origin = Vector2{ 0.5f, 0.0f };
            layout.Clear();

            UI::Button start_button = UI::Button(&style, &font, "Start", font_size);
            UI::Button exit_button = UI::Button(&style, &font, "Exit", font_size);
            UI::Button join_button = UI::Button(&style, &font, "Join", font_size);
            static UI::TextField ip_field = UI::TextField(&style, &font, 15, "localhost", font_size);
            static UI::TextField port_field = UI::TextField(&style, &font, 5, "25565", font_size);

            layout.AddElement(&start_button.base);
            layout.EndRow();
            layout.AddElement(&ip_field.base);
            layout.AddElement(&port_field.base);
            layout.AddElement(&join_button.base);
            layout.EndRow();
            layout.AddElement(&exit_button.base);
            layout.EndRow();

            layout.EndColumn();

            if(start_button.IsReleased())
                active_window = GAME;

            ip_field.Update();
            port_field.Update();

            if(join_button.IsReleased())
            {
                // nothing done yet
            }

            if(exit_button.IsReleased())
                exit_window = true;

            BeginDrawing();
                ClearBackground(Color{20, 20, 20});

                start_button.Draw();
                join_button.Draw();
                exit_button.Draw();

                ip_field.Draw();
                port_field.Draw();
            EndDrawing();
        }
        else if(active_window == GAME)
        {
            if(IsKeyPressed(KEY_ESCAPE))
                active_window = MAIN_MENU;

            input.Update();
            player->Control(&input);
            player->onGround = 0;
            float deltaTime = GetFrameTime() > 0.02f ? 0.02f : GetFrameTime(); //will broke if < 60 FPS
            world.CalculateCollisions(*player, deltaTime);
            world.Update(deltaTime);

            BeginDrawing();
                ClearBackground(Color{25, 30, 40});
                world.Draw();
            EndDrawing();
        }
        else
        {
            break;
        }
    }

    CloseWindow();

    return 0;
}