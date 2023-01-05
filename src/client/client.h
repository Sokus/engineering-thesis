#ifndef CLIENT_H
#define CLIENT_H

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
    GM_LEVEL_MENU,
    GM_JOIN_MENU,
    GM_CONNECTING_MENU,
    GM_HOST_MENU,
    GM_OPTIONS_MENU,
    GM_PLAYER_MENU
};

struct GameState
{
    GameScene last_scene = GS_INVALID;
    GameScene current_scene = GS_TITLE_SCREEN;
    bool scene_changed = true;
    GameMenu last_menu = GM_NONE;
    GameMenu current_menu = GM_MAIN_MENU;
    bool menu_changed = true;
    bool should_quit = false;

    char join_ip_field[16];
    char join_port_field[6];
    char host_port_field[6];
};

#endif // CLIENT_H