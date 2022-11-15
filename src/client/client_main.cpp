#include <stdio.h>
#include <stdlib.h>

#include "glm/glm.hpp"

#include "config.h"
#include "macros.h" // ABS
#include "game/game.h"
#include "game/input.h"

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
    SetTargetFPS(60);

    Game::Input input;
    Game::World world;

    character = LoadTexture(RESOURCE_PATH "/character.png");

    Game::Entity *player = world.CreatePlayer(100.0f, 100.0f);
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
            const int font_size = 40;
            float spacing = 2.0f;
            Vector2 button_padding = { 40.0f, 10.0f };
            const float button_spacing = 15.0f;
            Color button_color_default = LIGHTGRAY;
            Color button_color_hover = WHITE;
            Color text_color = BLACK;

            const char *start_text = "Start";
            Vector2 start_text_size = MeasureTextEx(font, start_text, font_size, spacing);

            const char *exit_text = "Exit";
            Vector2 exit_text_size = MeasureTextEx(font, exit_text, font_size, spacing);

            Vector2 max_text_size;
            max_text_size.x = MAX(start_text_size.x, exit_text_size.x);
            max_text_size.y = start_text_size.y;

            float rectangle_width = max_text_size.x + 2.0f*button_padding.x;
            float rectangle_height = max_text_size.y + 2.0f*button_padding.y;

            Vector2 menu_position = {
                (float)GetScreenWidth()/2.0f - rectangle_width/2.0f,
                100.0f
            };
            float menu_vert_offset = 0.0f;

            Rectangle start_button = {
                menu_position.x,
                menu_position.y + menu_vert_offset,
                rectangle_width,
                rectangle_height
            };
            menu_vert_offset += rectangle_height + button_spacing;


            Rectangle exit_button = {
                menu_position.x,
                menu_position.y + menu_vert_offset,
                rectangle_width,
                rectangle_height
            };
            menu_vert_offset += rectangle_height + button_spacing;

            Color start_button_color;
            Color exit_button_color;

            Vector2 mouse_position = GetMousePosition();

            if(CheckCollisionPointRec(mouse_position, start_button))
            {
                start_button_color = button_color_hover;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    active_window = GAME;
            }
            else
            {
                start_button_color = button_color_default;
            }

            if(CheckCollisionPointRec(mouse_position, exit_button))
            {
                exit_button_color = button_color_hover;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    exit_window = true;
            }
            else
            {
                exit_button_color = button_color_default;
            }

            BeginDrawing();
                ClearBackground(Color{20, 20, 20});

                DrawRectangleRec(start_button, start_button_color);
                Vector2 start_text_pos = {
                    start_button.x + button_padding.x + (max_text_size.x - start_text_size.x)/2.0f,
                    start_button.y + button_padding.y + (max_text_size.y - start_text_size.y)/2.0f,
                };
                DrawTextEx(font, start_text, start_text_pos, font_size, spacing, text_color);

                DrawRectangleRec(exit_button, exit_button_color);
                Vector2 exit_text_pos = {
                    exit_button.x + button_padding.x + (max_text_size.x - exit_text_size.x)/2.0f,
                    exit_button.y + button_padding.y + (max_text_size.y - exit_text_size.y)/2.0f,
                };
                DrawTextEx(font, exit_text, exit_text_pos, font_size, spacing, text_color);

            EndDrawing();
        }
        else if(active_window == GAME)
        {
            input.Update();
            player->Control(&input);
            world.Update(GetFrameTime());

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