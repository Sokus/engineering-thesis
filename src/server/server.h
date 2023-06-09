#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#include "system/network.h"
#include "protocol/protocol.h"
#include "game/input.h"
#include "game/world.h"

struct ServerClientData
{
    uint64_t connect_time;
    uint64_t last_packet_send_time;
    uint64_t last_packet_receive_time;
};

struct Server
{
    Socket socket;
    int num_connected_clients;
    bool client_connected[MAX_CLIENTS];
    Address client_address[MAX_CLIENTS];
    ServerClientData client_data[MAX_CLIENTS];
    Game::Input client_input[MAX_CLIENTS];

    Game::World world;
    Game::LevelType level_type;

    void Init(Socket socket);
    void ResetClientState(int client_index);
    void SendPackets();
    void ReceivePackets();
    void SendPacketToConnectedClient(int client_index, Packet *packet);
    void BroadcastPacketToConnectedClients(Packet *packet);
    int FindFreeClientIndex();
    int FindExistingClientIndex(Address address);
    void ConnectClient(int client_index, Game::PlayerType player_type, Address address);
    void DisconnectClient(int client_index);
    void CheckForTimeOut();
    void SendWorldState();
    void SendQueuedParticles();
    void GameFinished();

    void ProcessConnectionRequestPacket(ConnectionRequestPacket *packet, Address address);
    void ProcessConnectionDisconnectPacket(ConnectionDisconnectPacket *packet, Address address);
    void ProcessConnectionKeepAlivePacket(ConnectionKeepAlivePacket *packet, Address address);
    void ProcessInputPacket(InputPacket *packet, Address address);
};

#endif // SERVER_H