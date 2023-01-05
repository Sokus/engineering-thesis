#ifndef CLIENT_H
#define CLIENT_H

#include "game/world.h"

enum GameScene
{
    GAME_SCENE_INVALID,
    GAME_SCENE_TITLE_SCREEN,
    GAME_SCENE_GAME
};

enum GameMenu
{
    GAME_MENU_NONE,
    GAME_MENU_MAIN,
    GAME_MENU_LEVEL,
    GAME_MENU_JOIN,
    GAME_MENU_CONNECTING,
    GAME_MENU_HOST,
    GAME_MENU_OPTIONS,
    GAME_MENU_PLAYER_SELECTION
};

struct GameState
{
    GameScene last_scene;
    GameScene current_scene;
    bool scene_changed;
    GameMenu last_menu;
    GameMenu current_menu;
    bool menu_changed;
    bool should_quit;
    bool multiplayer;

    Game::PlayerType player_type_selected;
    Game::Level *level_selected;
    char join_ip_field[16];
    char join_port_field[6];
    char host_port_field[6];

    Game::World world;
};

#endif // CLIENT_H