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
    SetExitKey(KEY_END);

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
            Vector2 button_padding = { 10.0f, 10.0f };
            Vector2 button_spacing = { 10.0f, 10.0f };
            Color button_color_default = LIGHTGRAY;
            Color button_color_hover = WHITE;
            Color button_color_active_lines = RED;
            Color text_color = BLACK;

            const char *start_text = "Start";
            Vector2 start_text_size = MeasureTextEx(font, start_text, font_size, spacing);

            const char *exit_text = "Exit";
            Vector2 exit_text_size = MeasureTextEx(font, exit_text, font_size, spacing);

            Vector2 ip_field_size = MeasureTextEx(font, "xxx.xxx.xxx.xxx", font_size, spacing);
            Vector2 port_field_size = MeasureTextEx(font, "xxxxx", font_size, spacing);
            const char *join_text = "Join";
            Vector2 join_text_size = MeasureTextEx(font, join_text, font_size, spacing);
            float join_widget_width = ip_field_size.x + port_field_size.x + join_text_size.x + 2.0f*button_spacing.x + 4.0f*button_padding.x;

            float max_button_width = MAX(MAX(start_text_size.x, exit_text_size.x), join_widget_width);
            float max_button_height = start_text_size.y;

            float rectangle_width = max_button_width + 2.0f*button_padding.x;
            float rectangle_height = max_button_height + 2.0f*button_padding.y;

            Vector2 menu_position = {
                (float)GetScreenWidth()/2.0f - rectangle_width/2.0f,
                100.0f
            };
            Vector2 menu_offset = { 0.0f, 0.0f };

            Rectangle start_button = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                rectangle_width,
                rectangle_height
            };
            menu_offset.y += rectangle_height + button_spacing.y;

            Rectangle ip_field = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                ip_field_size.x + 2.0f*button_padding.x,
                ip_field_size.y + 2.0f*button_padding.y
            };
            menu_offset.x += ip_field.width + button_spacing.x;

            Rectangle port_field = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                port_field_size.x + 2.0f*button_padding.x,
                port_field_size.y + 2.0f*button_padding.y
            };
            menu_offset.x += port_field.width + button_spacing.x;

            Rectangle join_button = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                join_text_size.x + 2.0f*button_padding.x,
                join_text_size.y + 2.0f*button_padding.y
            };
            menu_offset.x = 0.0f;
            menu_offset.y += join_button.height + button_spacing.y;

            Rectangle exit_button = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                rectangle_width,
                rectangle_height
            };
            menu_offset.y += rectangle_height + button_spacing.y;


            Vector2 mouse_position = GetMousePosition();

            Color start_button_color;
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

            Color ip_field_color;
            static bool ip_field_active = false;
            if(CheckCollisionPointRec(mouse_position, ip_field))
            {
                ip_field_color = button_color_hover;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    ip_field_active = true;
            }
            else
            {
                ip_field_color = button_color_default;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    ip_field_active = false;
            }

            const int max_ip_chars = 15;
            static char ip_chars[max_ip_chars + 1] = "\0";
            static int ip_char_count = 0;
            if(ip_field_active)
            {
                int key = GetCharPressed();
                while(key > 0)
                {
                    if((key >= 32) && (key <= 125) && (ip_char_count < max_ip_chars))
                    {
                        ip_chars[ip_char_count] = (char)key;
                        ip_chars[ip_char_count + 1] = '\0';
                        ip_char_count++;
                    }

                    key = GetCharPressed();
                }

                if(IsKeyPressed(KEY_BACKSPACE))
                {
                    ip_char_count--;
                    if(ip_char_count < 0) ip_char_count = 0;
                    ip_chars[ip_char_count] = '\0';
                }
            }

            Color port_field_color;
            static bool port_field_active = false;
            if(CheckCollisionPointRec(mouse_position, port_field))
            {
                port_field_color = button_color_hover;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    port_field_active = true;
            }
            else
            {
                port_field_color = button_color_default;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    port_field_active = false;
            }

            const int max_port_chars = 5;
            static char port_chars[max_port_chars + 1] = "\0";
            static int port_char_count = 0;
            if(port_field_active)
            {
                int key = GetCharPressed();
                while(key > 0)
                {
                    if((key >= 32) && (key <= 125) && (port_char_count < max_port_chars))
                    {
                        port_chars[port_char_count] = (char)key;
                        port_chars[port_char_count + 1] = '\0';
                        port_char_count++;
                    }

                    key = GetCharPressed();
                }

                if(IsKeyPressed(KEY_BACKSPACE))
                {
                    port_char_count--;
                    if(port_char_count < 0) port_char_count = 0;
                    port_chars[port_char_count] = '\0';
                }
            }

            Color join_button_color;
            if(CheckCollisionPointRec(mouse_position, join_button))
            {
                join_button_color = button_color_hover;
            }
            else
            {
                join_button_color = button_color_default;
            }

            Color exit_button_color;
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
                    start_button.x + button_padding.x + (max_button_width - start_text_size.x)/2.0f,
                    start_button.y + button_padding.y + (max_button_height - start_text_size.y)/2.0f,
                };
                DrawTextEx(font, start_text, start_text_pos, font_size, spacing, text_color);

                DrawRectangleRec(ip_field, ip_field_color);
                if(ip_field_active)
                {
                    DrawRectangleLinesEx(ip_field, 2.0f, button_color_active_lines);
                }
                Vector2 ip_field_text_pos = {
                    ip_field.x + button_padding.x,
                    ip_field.y + button_padding.y,
                };
                DrawTextEx(font, ip_chars, ip_field_text_pos, font_size, spacing, text_color);

                DrawRectangleRec(port_field, port_field_color);
                if(port_field_active)
                {
                    DrawRectangleLinesEx(port_field, 2.0f, button_color_active_lines);
                }
                Vector2 port_field_text_pos = {
                    port_field.x + button_padding.x,
                    port_field.y + button_padding.y,
                };
                DrawTextEx(font, port_chars, port_field_text_pos, font_size, spacing, text_color);

                DrawRectangleRec(join_button, join_button_color);
                Vector2 join_text_pos = {
                    join_button.x + button_padding.x,
                    join_button.y + button_padding.y,
                };
                DrawTextEx(font, join_text, join_text_pos, font_size, spacing, text_color);

                DrawRectangleRec(exit_button, exit_button_color);
                Vector2 exit_text_pos = {
                    exit_button.x + button_padding.x + (max_button_width - exit_text_size.x)/2.0f,
                    exit_button.y + button_padding.y + (max_button_height - exit_text_size.y)/2.0f,
                };
                DrawTextEx(font, exit_text, exit_text_pos, font_size, spacing, text_color);

            EndDrawing();
        }
        else if(active_window == GAME)
        {
            if(IsKeyPressed(KEY_ESCAPE))
                active_window = MAIN_MENU;

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