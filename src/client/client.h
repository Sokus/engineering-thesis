#ifndef CLIENT_H
#define CLIENT_H

#include "game/world.h"
#include "system/network.h"
#include "protocol/protocol.h"

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
    GAME_MENU_PLAYER_SELECTION,
    GAME_MENU_WON
};

enum ClientState
{
    CLIENT_DISCONNECTED,
    CLIENT_SENDING_CONNECTION_REQUEST,
    CLIENT_CONNECTED,
    CLIENT_CONNECTION_REQUEST_TIMED_OUT,
    CLIENT_KEEP_ALIVE_TIMED_OUT,
    CLIENT_CONNECTION_DENIED_FULL,
};

struct Client
{
    ClientState state;
    Socket socket;
    Address server_address;
    int client_index;
    uint64_t last_packet_send_time;
    uint64_t last_packet_receive_time;

    void Init(Socket socket);
    void ResetConnectionData();
    void Connect(Address address);
    void Disconnect();
    void SendPacketToServer(Packet *packet);
    void SendPackets();
    void ReceivePackets();
    void CheckForTimeOut();

    void ProcessConnectionAcceptedPacket(ConnectionAcceptedPacket *packet, Address address);
    void ProcessConnectionDeniedPacket(ConnectionDeniedPacket *packet, Address address);
    void ProcessConnectionDisconnectPacket(ConnectionDisconnectPacket *packet, Address address);
    void ProcessConnectionKeepAlivePacket(ConnectionKeepAlivePacket *packet, Address address);
};

struct GameData
{
    Game::World world;
};

struct AppData
{
    GameScene last_scene;
    GameScene current_scene;
    bool scene_changed;
    GameMenu last_menu;
    GameMenu current_menu;
    bool menu_changed;
    bool should_quit;

    char join_ip_field[16];
    char join_port_field[6];
    char host_port_field[6];
    Game::PlayerType player_type_selected;
    Game::Level *level_selected;
};

#endif // CLIENT_H