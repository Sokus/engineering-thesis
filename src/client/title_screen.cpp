#include "title_screen.h"

#include "client.h"
#include "user_interface.h"
#include "game/world.h"
#include "macros.h"
#include "system.h"
#include "server_control.h"
#include "raylib.h"

#include <string.h>

extern AppData app_state;
extern Client client_state;

extern Texture2D backdrop;

void DoMainMenu()
{
    UI::Button solo_play_button = UI::Button("Solo Play");
    UI::Button join_game_button = UI::Button("Join Multiplayer");
    UI::Button host_game_button = UI::Button("Host Game");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&solo_play_button.base); UI::EndRow();
        UI::Add(&join_game_button.base);
        UI::Add(&host_game_button.base); UI::EndRow();
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if (solo_play_button.IsReleased())
    {
        app_state.multiplayer = false;
        app_state.current_menu = GAME_MENU_LEVEL;
    }

    if (join_game_button.IsReleased())
        app_state.current_menu = GAME_MENU_JOIN;

    if (host_game_button.IsReleased())
        app_state.current_menu = GAME_MENU_HOST;

    if (exit_button.IsReleased())
        app_state.should_quit = true;

    solo_play_button.Draw();
    join_game_button.Draw();
    host_game_button.Draw();
    exit_button.Draw();
}

void DoJoinMenu()
{
    if(app_state.menu_changed)
    {
        const char default_ip[] = "127.0.0.1";
        const char default_port[] = "50000";
        memcpy(app_state.join_ip_field, default_ip, sizeof(default_ip));
        memcpy(app_state.join_port_field, default_port, sizeof(default_port));
    }

    UI::TextField ip_field = UI::TextField(app_state.join_ip_field, sizeof(app_state.join_ip_field));
    UI::TextField port_field = UI::TextField(app_state.join_port_field, sizeof(app_state.join_port_field));
    UI::Button join_button = UI::Button("Join");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&ip_field.base);
        UI::Add(&port_field.base);
        UI::Add(&join_button.base); UI::EndRow();
        UI::AddSpace(); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    ip_field.Update();
    port_field.Update();

    if(join_button.IsReleased())
    {
        char address_string[255] = "";
        int index = 0;
        TextAppend(address_string, app_state.join_ip_field, &index);
        TextAppend(address_string, ":", &index);
        TextAppend(address_string, app_state.join_port_field, &index);
        Address address = AddressParse(address_string);
        if (AddressIsValid(address))
        {
            app_state.multiplayer = true;
            app_state.server_address = address;
            app_state.current_menu = GAME_MENU_PLAYER_SELECTION;
        }
    }

    if(close_button.IsReleased())
        app_state.current_menu = GAME_MENU_MAIN;

    ip_field.Draw();
    port_field.Draw();
    join_button.Draw();
    close_button.Draw();
}

void DoConnectingMenu()
{
    DrawText("Connecting...", 20, 20, 20, WHITE);

    if (app_state.menu_changed)
    {
        client_state.Connect(app_state.server_address, app_state.player_type_selected);
    }

    if (client_state.state == CLIENT_CONNECTED)
    {
        app_state.current_menu = GAME_MENU_NONE;
        app_state.current_scene = GAME_SCENE_GAME;
    }
    else if (client_state.state == CLIENT_ERROR || client_state.state == CLIENT_DISCONNECTED)
    {
        app_state.current_menu = GAME_MENU_MAIN;
    }
}

void DoHostMenu()
{
    if(app_state.menu_changed)
    {
        const char default_port[] = "50000";
        memcpy(app_state.host_port_field, default_port, sizeof(default_port));
    }

    UI::TextField port_field = UI::TextField(app_state.host_port_field, sizeof(app_state.host_port_field));
    UI::Button host_button = UI::Button("Host");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&port_field.base);
        UI::Add(&host_button.base); UI::EndRow();
        UI::AddSpace(); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    port_field.Update();

    if(host_button.IsReleased())
    {
        char address_string[255] = "127.0.0.1:";
        int address_length = TextLength(address_string);
        TextAppend(address_string, app_state.host_port_field, &address_length);
        Address address = AddressParse(address_string);
        if (AddressIsValid(address))
        {
            app_state.multiplayer = true;
            app_state.server_address = address;
            app_state.current_menu = GAME_MENU_LEVEL;
        }
    }

    if(close_button.IsReleased())
        app_state.current_menu = GAME_MENU_MAIN;

    port_field.Draw();
    host_button.Draw();
    close_button.Draw();
}

void DoLevelMenu()
{
    struct ButtonTypePair { UI::Button button; Game::LevelType level_type; };
    ButtonTypePair buttons[] = {
        { UI::Button("Level 1"), Game::LEVEL_PLAINS },
        { UI::Button("Level 2"), Game::LEVEL_LAKE },
        { UI::Button("Level 3"), Game::LEVEL_CLOUDS }};
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        for (int i = 0; i < ARRAY_SIZE(buttons); i++)
            UI::Add(&buttons[i].button.base);
        UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        app_state.current_menu = GAME_MENU_MAIN;

    for (int i = 0; i < ARRAY_SIZE(buttons); i++) {
        if (buttons[i].button.IsReleased()) {
            app_state.level_type_selected = buttons[i].level_type;
            app_state.current_menu = GAME_MENU_PLAYER_SELECTION;

            if (app_state.multiplayer)
            {
                LaunchServer(app_state.server_address.port, app_state.level_type_selected);
            }
        }
        buttons[i].button.Draw();
    }

    close_button.Draw();
}

void DoPlayerSelectionMenu()
{
    struct ButtonTypePair { UI::Button button; Game::PlayerType player_type; };
    ButtonTypePair buttons[] = {
        { UI::Button("Rouge"), Game::PLAYER_TYPE_ROUGE },
        { UI::Button("Sniper"), Game::PLAYER_TYPE_SNIPER },
        { UI::Button("Healer"), Game::PLAYER_TYPE_HEALER },
        { UI::Button("Warrior"), Game::PLAYER_TYPE_WARRIOR }};
    UI::Button close_button = UI::Button("Return");

    UI::Begin();
    {
        for (int i = 0; i < ARRAY_SIZE(buttons); i++)
            UI::Add(&buttons[i].button.base);
        UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        app_state.current_menu = GAME_MENU_LEVEL;

    for (int i = 0; i < ARRAY_SIZE(buttons); i++) {
        if (buttons[i].button.IsReleased()) {
            app_state.player_type_selected = buttons[i].player_type;

            if (app_state.multiplayer)
            {
                app_state.current_menu = GAME_MENU_CONNECTING;
            }
            else
            {
                app_state.current_menu = GAME_MENU_NONE;
                app_state.current_scene = GAME_SCENE_GAME;
            }
        }
        buttons[i].button.Draw();
    }

    close_button.Draw();
}


void DoTitleScreenScene()
{
    if(app_state.current_menu == GAME_MENU_NONE)
        app_state.current_menu = GAME_MENU_MAIN;

    DrawTexture(backdrop, 0, 0, WHITE);

    UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
    UI::SetOrigin(0.5f, 0.5f);

    ClearBackground(Color{20, 20, 20});
    switch(app_state.current_menu)
    {
        case GAME_MENU_MAIN: DoMainMenu(); break;
        case GAME_MENU_JOIN: DoJoinMenu(); break;
        case GAME_MENU_CONNECTING: DoConnectingMenu(); break;
        case GAME_MENU_HOST: DoHostMenu(); break;
        case GAME_MENU_LEVEL: DoLevelMenu(); break;
        case GAME_MENU_PLAYER_SELECTION: DoPlayerSelectionMenu(); break;
        case GAME_MENU_WON: DoLevelMenu(); break;
        default: break;
    }
}