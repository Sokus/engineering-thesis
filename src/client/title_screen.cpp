#include "title_screen.h"

#include "client.h"
#include "user_interface.h"
#include "game/world.h"

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
        game_state.current_menu = GM_LEVEL_MENU;

    if (join_game_button.IsReleased())
        game_state.current_menu = GM_JOIN_MENU;

    if (host_game_button.IsReleased())
        game_state.current_menu = GM_HOST_MENU;

    if (options_button.IsReleased())
        game_state.current_menu = GM_OPTIONS_MENU;

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
        game_state.current_menu = GM_CONNECTING_MENU;

    if(close_button.IsReleased())
        game_state.current_menu = GM_MAIN_MENU;



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
        game_state.current_menu = GM_MAIN_MENU;

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
        game_state.current_menu = GM_MAIN_MENU;
    if (lvl1_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        game_state.current_menu = GM_PLAYER_MENU;
    }
    if (lvl2_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        game_state.current_menu = GM_PLAYER_MENU;
    }
    if (lvl3_button.IsReleased()) {
        Game::ActualLevel = Game::plains;
        game_state.current_menu = GM_PLAYER_MENU;
    }

    lvl1_button.Draw();
    lvl2_button.Draw();
    lvl3_button.Draw();
    close_button.Draw();
}

void DoPlayerMenu()
{
    UI::Button player1_button = UI::Button("Rouge");
    UI::Button player2_button = UI::Button("Sniper");
    UI::Button player3_button = UI::Button("Healer");
    UI::Button player4_button = UI::Button("Warrior");
    UI::Button close_button = UI::Button("Return");

    UI::Begin();
    {
        UI::Add(&player1_button.base);
        UI::Add(&player2_button.base);
        UI::Add(&player3_button.base);
        UI::Add(&player4_button.base); UI::EndRow();
        UI::Add(&close_button.base); UI::EndRow();
    }
    UI::End();

    if (close_button.IsReleased())
        game_state.current_menu = GM_LEVEL_MENU;
    if (player1_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_ROUGE;
        game_state.current_menu = GM_NONE;
        game_state.current_scene = GS_GAME;
    }
    if (player2_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_SNIPER;
        game_state.current_menu = GM_NONE;
        game_state.current_scene = GS_GAME;
    }
    if (player3_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_HEALER;
        game_state.current_menu = GM_NONE;
        game_state.current_scene = GS_GAME;
    }
    if (player4_button.IsReleased()) {
        Game::ActualPlayer = Game::PLAYER_TYPE_WARRIOR;
        game_state.current_menu = GM_NONE;
        game_state.current_scene = GS_GAME;
    }

    player1_button.Draw();
    player2_button.Draw();
    player3_button.Draw();
    player4_button.Draw();
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
        game_state.current_menu = GM_MAIN_MENU;

    close_button.Draw();
}

void DoTitleScreenScene()
{
    if(game_state.current_menu == GM_NONE)
        game_state.current_menu = GM_MAIN_MENU;

    UI::SetPosition(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
    UI::SetOrigin(0.5f, 0.5f);

    ClearBackground(Color{20, 20, 20});
    switch(game_state.current_menu)
    {
        case GM_MAIN_MENU: DoMainMenu(); break;
        case GM_JOIN_MENU: DoJoinMenu(); break;
        case GM_CONNECTING_MENU: DoConnectingMenu(); break;
        case GM_HOST_MENU: DoHostMenu(); break;
        case GM_LEVEL_MENU: DoLevelMenu(); break;
        case GM_OPTIONS_MENU: DoOptionsMenu(); break;
        case GM_PLAYER_MENU: DoPlayerMenu(); break;
        default: break;
    }
}