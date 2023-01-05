#include "title_screen.h"

#include "client.h"
#include "user_interface.h"
#include "game/world.h"
#include "macros.h"

#include <string.h>

extern GameState game_state;

void DoMainMenu()
{
    UI::Button solo_play_button = UI::Button("Solo Play");
    UI::Button join_game_button = UI::Button("Join Multiplayer");
    UI::Button host_game_button = UI::Button("Host Game");
    UI::Button options_button = UI::Button("Options");
    UI::Button exit_button = UI::Button("Exit");

    UI::Begin();
    {
        UI::Add(&solo_play_button.base); UI::EndRow();
        UI::Add(&join_game_button.base);
        UI::Add(&host_game_button.base); UI::EndRow();
        UI::Add(&options_button.base); UI::EndRow();
        UI::Add(&exit_button.base); UI::EndRow();
    }
    UI::End();

    if (solo_play_button.IsReleased())
        game_state.current_menu = GAME_MENU_LEVEL;

    if (join_game_button.IsReleased())
        game_state.current_menu = GAME_MENU_JOIN;

    if (host_game_button.IsReleased())
        game_state.current_menu = GAME_MENU_HOST;

    if (options_button.IsReleased())
        game_state.current_menu = GAME_MENU_OPTIONS;

    if (exit_button.IsReleased())
        game_state.should_quit = true;

    solo_play_button.Draw();
    join_game_button.Draw();
    host_game_button.Draw();
    options_button.Draw();
    exit_button.Draw();
}

void DoJoinMenu()
{
    if(game_state.menu_changed)
    {
        const char default_ip[] = "localhost";
        const char default_port[] = "25565";
        memcpy(game_state.join_ip_field, default_ip, sizeof(default_ip));
        memcpy(game_state.join_port_field, default_port, sizeof(default_port));
    }

    UI::TextField ip_field = UI::TextField(game_state.join_ip_field, sizeof(game_state.join_ip_field));
    UI::TextField port_field = UI::TextField(game_state.join_port_field, sizeof(game_state.join_port_field));
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
        game_state.current_menu = GAME_MENU_CONNECTING;

    if(close_button.IsReleased())
        game_state.current_menu = GAME_MENU_MAIN;



    ip_field.Draw();
    port_field.Draw();
    join_button.Draw();
    close_button.Draw();
}

void DoConnectingMenu()
{
    if(game_state.menu_changed)
    {

    }

    ClearBackground(DARKGREEN);
}

void DoHostMenu()
{
    if(game_state.menu_changed)
    {
        const char default_port[] = "25565";
        memcpy(game_state.host_port_field, default_port, sizeof(default_port));
    }

    UI::TextField port_field = UI::TextField(game_state.host_port_field, sizeof(game_state.host_port_field));
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
        // nothing done yet
    }

    if(close_button.IsReleased())
        game_state.current_menu = GAME_MENU_MAIN;

    port_field.Draw();
    host_button.Draw();
    close_button.Draw();
}

void DoLevelMenu()
{
    UI::Button lvl1_button = UI::Button("Level 1");
    UI::Button lvl2_button = UI::Button("Level 2");
    UI::Button lvl3_button = UI::Button("Level 3");
    UI::Button close_button = UI::Button("Close");

    UI::Begin();
    {
        UI::Add(&lvl1_button.base);
        UI::Add(&lvl2_button.base);
        UI::Add(&lvl3_button.base); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        game_state.current_menu = GAME_MENU_MAIN;
    if (lvl1_button.IsReleased()) {
        game_state.level_selected = &Game::plains;
        game_state.current_menu = GAME_MENU_PLAYER_SELECTION;
    }
    if (lvl2_button.IsReleased()) {
        game_state.level_selected = &Game::plains;
        game_state.current_menu = GAME_MENU_PLAYER_SELECTION;
    }
    if (lvl3_button.IsReleased()) {
        game_state.level_selected = &Game::plains;
        game_state.current_menu = GAME_MENU_PLAYER_SELECTION;
    }

    lvl1_button.Draw();
    lvl2_button.Draw();
    lvl3_button.Draw();
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
        game_state.current_menu = GAME_MENU_LEVEL;

    for (int i = 0; i < ARRAY_SIZE(buttons); i++) {
        if (buttons[i].button.IsReleased()) {
            game_state.player_type_selected = buttons[i].player_type;
            game_state.current_menu = GAME_MENU_NONE;
            game_state.current_scene = GAME_SCENE_GAME;
        }
        buttons[i].button.Draw();
    }

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
        game_state.current_menu = GAME_MENU_MAIN;

    close_button.Draw();
}

void DoTitleScreenScene()
{
    if(game_state.current_menu == GAME_MENU_NONE)
        game_state.current_menu = GAME_MENU_MAIN;

    UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
    UI::SetOrigin(0.5f, 0.5f);

    ClearBackground(Color{20, 20, 20});
    switch(game_state.current_menu)
    {
        case GAME_MENU_MAIN: DoMainMenu(); break;
        case GAME_MENU_JOIN: DoJoinMenu(); break;
        case GAME_MENU_CONNECTING: DoConnectingMenu(); break;
        case GAME_MENU_HOST: DoHostMenu(); break;
        case GAME_MENU_LEVEL: DoLevelMenu(); break;
        case GAME_MENU_OPTIONS: DoOptionsMenu(); break;
        case GAME_MENU_PLAYER_SELECTION: DoPlayerSelectionMenu(); break;
        default: break;
    }
}