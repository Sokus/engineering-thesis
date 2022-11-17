#include <stdio.h>
#include <stdlib.h>

#include "glm/glm.hpp"

#include "config.h"
#include "macros.h" // ABS
#include "game/game.h"
#include "game/input.h"

#include "raylib.h"

// TODO: split this
#include "ui_elements.cpp"

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

            Style style = {};
            style.fill_default = LIGHTGRAY;
            style.fill_active = WHITE;
            style.text_default = DARKGRAY;
            style.text_suggestion = GRAY;
            style.outline_active = RED;
            style.padding = { 10.0f, 10.0f };
            style.spacing = { 10.0f, 10.0f };

            Button start_button = Button(&style, &font, "Start", font_size);
            Button exit_button = Button(&style, &font, "Exit", font_size);
            Button join_button = Button(&style, &font, "Join", font_size);

            Vector2 ip_field_size = MeasureTextEx(font, "xxx.xxx.xxx.xxx", font_size, 0.0f);
            Vector2 port_field_size = MeasureTextEx(font, "xxxxx", font_size, 0.0f);

            float join_widget_width = ip_field_size.x + port_field_size.x + join_button.label_size.x + 2.0f*style.spacing.x + 4.0f*style.padding.x;

            float max_button_width = MAX(MAX(start_button.label_size.x, exit_button.label_size.x), join_widget_width);
            float max_button_height = start_button.label_size.y;

            float rectangle_width = max_button_width + 2.0f*style.padding.x;
            float rectangle_height = max_button_height + 2.0f*style.padding.y;

            Vector2 menu_position = {
                (float)GetScreenWidth()/2.0f - rectangle_width/2.0f,
                100.0f
            };
            Vector2 menu_offset = { 0.0f, 0.0f };

            start_button.rect = Rectangle{
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                rectangle_width,
                rectangle_height
            };
            menu_offset.y += start_button.rect.height + style.spacing.y;

            Rectangle ip_field = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                ip_field_size.x + 2.0f*style.padding.x,
                ip_field_size.y + 2.0f*style.padding.y
            };
            menu_offset.x += ip_field.width + style.spacing.x;

            Rectangle port_field = {
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                port_field_size.x + 2.0f*style.padding.x,
                port_field_size.y + 2.0f*style.padding.y
            };
            menu_offset.x += port_field.width + style.spacing.x;

            join_button.rect = Rectangle{
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                join_button.label_size.x + 2.0f*style.padding.x,
                join_button.label_size.y + 2.0f*style.padding.y
            };
            menu_offset.x = 0.0f;
            menu_offset.y += join_button.rect.height + style.spacing.y;

            exit_button.rect = Rectangle{
                menu_position.x + menu_offset.x,
                menu_position.y + menu_offset.y,
                rectangle_width,
                rectangle_height
            };
            menu_offset.y += exit_button.rect.height + style.spacing.y;


            Vector2 mouse_position = GetMousePosition();

            if(start_button.IsReleased())
                active_window = GAME;

            Color ip_field_color;
            static bool ip_field_active = false;
            if(CheckCollisionPointRec(mouse_position, ip_field))
            {
                ip_field_color = style.fill_active;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    ip_field_active = true;
            }
            else
            {
                ip_field_color = style.fill_default;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    ip_field_active = false;
            }

            const int max_ip_chars = 15;
            const char *ip_default = "localhost";
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
                port_field_color = style.fill_active;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    port_field_active = true;
            }
            else
            {
                port_field_color = style.fill_default;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    port_field_active = false;
            }

            const int max_port_chars = 5;
            static char port_chars[max_port_chars + 1] = "\0";
            const char *port_default = "25565";
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

                DrawRectangleRec(ip_field, ip_field_color);
                if(ip_field_active)
                {
                    DrawRectangleLinesEx(ip_field, 2.0f, style.outline_active);
                }
                Vector2 ip_field_text_pos = {
                    ip_field.x + style.padding.x,
                    ip_field.y + style.padding.y,
                };
                const char *ip_displayed_text = ip_char_count ? ip_chars : ip_default;
                Color ip_text_color = ip_char_count ? style.text_default : style.text_suggestion;
                DrawTextEx(font, ip_displayed_text, ip_field_text_pos, font_size, 0.0f, ip_text_color);

                DrawRectangleRec(port_field, port_field_color);
                if(port_field_active)
                {
                    DrawRectangleLinesEx(port_field, 2.0f, style.outline_active);
                }
                Vector2 port_field_text_pos = {
                    port_field.x + style.padding.x,
                    port_field.y + style.padding.y,
                };
                const char *port_displayed_text = port_char_count ? port_chars : port_default;
                Color port_text_color = port_char_count ? style.text_default : style.text_suggestion;
                DrawTextEx(font, port_displayed_text, port_field_text_pos, font_size, 0.0f, port_text_color);


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