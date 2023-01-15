#include "server.h"

#include <string.h>

#include "macros.h"
#include "system/pi_time.h"
#include "game/level.h"

void Server::Init(Socket socket)
{
    memset(this, 0, sizeof(Server));
    this->socket = socket;
    for (int i = 1; i < MAX_CLIENTS; i++)
        ResetClientState(i);
    world.Clear();
    Game::SetLoadTextures(false);
    Game::InitLevel(&world, Game::LEVEL_PLAINS);
}

void Server::ResetClientState(int client_index)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    if (!client_index) return;

    client_connected[client_index] = false;
    client_address[client_index] = Address{};
    client_data[client_index] = ServerClientData{};
    client_input[client_index] = Game::Input{};
}

void Server::SendPackets()
{
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (!client_connected[i])
            continue;

        double time_since_last_packet_received = Time_Sec(Time_Since(client_data[i].last_packet_receive_time));
        if (time_since_last_packet_received < CONNECTION_KEEP_ALIVE_SEND_RATE)
            continue;

        ConnectionKeepAlivePacket *packet = (ConnectionKeepAlivePacket *)CreatePacket(PACKET_CONNECTION_KEEP_ALIVE);
        SendPacketToConnectedClient(i, packet);
        DestroyPacket(packet);
    }
}

void Server::ReceivePackets()
{
    while (true)
    {
        Address address;
        Packet *packet = ReceivePacket(socket, &address);
        if (!packet) break;

        switch (packet->type)
        {
            case PACKET_CONNECTION_REQUEST:
                ProcessConnectionRequestPacket((ConnectionRequestPacket *)packet, address);
                break;

            case PACKET_CONNECTION_DISCONNECT:
                ProcessConnectionDisconnectPacket((ConnectionDisconnectPacket *)packet, address);
                break;

            case PACKET_CONNECTION_KEEP_ALIVE:
                ProcessConnectionKeepAlivePacket((ConnectionKeepAlivePacket *)packet, address);
                break;

            case PACKET_INPUT:
                ProcessInputPacket((InputPacket *)packet, address);
                break;

            default: break;
        }

        DestroyPacket(packet);
    }
}

void Server::SendPacketToConnectedClient(int client_index, Packet *packet)
{
    ASSERT(packet);
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    if (!client_index) return;
    ASSERT(client_connected[client_index]);
    SendPacket(socket, client_address[client_index], packet);
    client_data[client_index].last_packet_send_time = Time_Now();
}

void Server::BroadcastPacketToConnectedClients(Packet *packet)
{
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (client_connected[i])
            SendPacketToConnectedClient(i, packet);
    }
}

int Server::FindFreeClientIndex()
{
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (!client_connected[i])
            return i;
    }
    return 0;
}

int Server::FindExistingClientIndex(Address address)
{
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (client_connected[i] && AddressCompare(client_address[i], address))
            return i;
    }
    return 0;
}

void Server::ConnectClient(int client_index, Address address)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    ASSERT(num_connected_clients >= 0);
    ASSERT(num_connected_clients < MAX_CLIENTS - 2);
    if (!client_index) return;
    ASSERT(!client_connected[client_index]);

    num_connected_clients++;

    client_connected[client_index] = true;
    client_address[client_index] = address;
    client_data[client_index].connect_time = Time_Now();
    client_data[client_index].last_packet_send_time = Time_Now();
    client_data[client_index].last_packet_receive_time = Time_Now();

    char buffer[256];
    char *address_string = AddressToString(address, buffer, sizeof(buffer));
    printf("client %d connected (address = %s)\n", client_index, address_string);

    ConnectionAcceptedPacket *packet = (ConnectionAcceptedPacket *)CreatePacket(PACKET_CONNECTION_ACCEPTED);
    packet->client_index = client_index;
    SendPacketToConnectedClient(client_index, packet);
    DestroyPacket(packet);

    // TODO: Remove hardcoded spawnpoint
    world.CreatePlayer(client_index, 0.0f, 100.0f, Game::PLAYER_TYPE_ROUGE);
}

void Server::DisconnectClient(int client_index)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    ASSERT(num_connected_clients > 0);
    if (!client_index) return;
    ASSERT(client_connected[client_index]);

    char buffer[256];
    char *address_string = AddressToString(client_address[client_index], buffer, sizeof(buffer));
    printf("client %d disconnected (address = %s)\n", client_index, address_string);

    ConnectionDisconnectPacket *packet = (ConnectionDisconnectPacket *)CreatePacket(PACKET_CONNECTION_DISCONNECT);
    SendPacketToConnectedClient(client_index, packet);
    DestroyPacket(packet);

    world.FreeOwnedEntities(client_index);

    ResetClientState(client_index);
    num_connected_clients--;
}

void Server::CheckForTimeOut()
{
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (!client_connected[i])
            continue;

        double time_since_last_packet_received = Time_Sec(Time_Since(client_data[i].last_packet_receive_time));
        if (time_since_last_packet_received > CONNECTION_KEEP_ALIVE_TIME_OUT)
        {
            char buffer[256];
            char *address_string = AddressToString(client_address[i], buffer, sizeof(buffer));
            printf("client %d timed out (address = %s)\n", i, address_string);
            DisconnectClient(i);
        }
    }
}

void Server::SendWorldState()
{
    if (!num_connected_clients)
        return;

    WorldStatePacket *packet = (WorldStatePacket *)CreatePacket(PACKET_WORLD_STATE);

    BitStream stream = MeasureStream_Create(nullptr, MAX_PACKET_SIZE);
    for (int i = 0; i < Game::max_entity_count; i++)
    {
        world.entities[i].Serialize(&stream);
        if (BitStream_GetBitsRemaining(&stream) < 0)
        {
            BroadcastPacketToConnectedClients(packet);

            stream = MeasureStream_Create(nullptr, MAX_PACKET_SIZE);
            world.entities[i].Serialize(&stream);
            ASSERT(BitStream_GetBitsRemaining(&stream) >= 0);

            packet->start_index = i;
            packet->entity_count = 0;
        }

        memcpy(&packet->entities[packet->entity_count], &world.entities[i], sizeof(Game::Entity));
        packet->entity_count++;
    }

    ASSERT(packet->entity_count > 0);
    BroadcastPacketToConnectedClients(packet);
    DestroyPacket(packet);
}

void Server::ProcessConnectionRequestPacket(ConnectionRequestPacket *packet, Address address)
{
    int existing_client_index = FindExistingClientIndex(address);
    if (existing_client_index)
    {
        ASSERT(existing_client_index >= 0);
        ASSERT(existing_client_index < MAX_CLIENTS);

        double time_since_last_sent_packet = Time_Sec(Time_Since(client_data[existing_client_index].last_packet_send_time));
        if (time_since_last_sent_packet >= CONNECTION_CONFIRM_SEND_RATE)
        {
            ConnectionKeepAlivePacket *packet = (ConnectionKeepAlivePacket *)CreatePacket(PACKET_CONNECTION_KEEP_ALIVE);
            SendPacketToConnectedClient(existing_client_index, packet);
            DestroyPacket(packet);
        }

        return;
    }

    char buffer[256];
    char *address_string = AddressToString(address, buffer, sizeof(buffer));
    printf("processing connection request packet from: %s\n", address_string);

    if (num_connected_clients == MAX_CLIENTS)
    {
        printf("connection denied: server is full\n");
        ConnectionDeniedPacket *packet = (ConnectionDeniedPacket *)CreatePacket(PACKET_CONNECTION_DENIED);
        packet->reason = CONNECTION_DENIED_SERVER_FULL;
        SendPacket(socket, address, packet);
        DestroyPacket(packet);
        return;
    }

    int client_index = FindFreeClientIndex();

    ASSERT(client_index);
    if (!client_index)
        return;

    ConnectClient(client_index, address);
}

void Server::ProcessConnectionDisconnectPacket(ConnectionDisconnectPacket *packet, Address address)
{
    int client_index = FindExistingClientIndex(address);
    if (!client_index)
        return;

    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);

    DisconnectClient(client_index);
}

void Server::ProcessConnectionKeepAlivePacket(ConnectionKeepAlivePacket *packet, Address address)
{
    int client_index = FindExistingClientIndex(address);
    if (!client_index)
        return;

    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);

    client_data[client_index].last_packet_receive_time = Time_Now();
}

void Server::ProcessInputPacket(InputPacket *packet, Address address)
{
    int client_index = FindExistingClientIndex(address);
    if (!client_index)
        return;

    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);

    client_input[client_index] = packet->input;
}
