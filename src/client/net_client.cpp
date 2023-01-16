#include "client.h"

#include <string.h>

#include "system/pi_time.h"
#include "macros.h"

extern GameData game_data;

void Client::Init(Socket socket)
{
    memset(this, 0, sizeof(Client));
    this->socket = socket;
    ResetConnectionData();
}

void Client::ResetConnectionData()
{
    state = CLIENT_DISCONNECTED;
    server_address = Address{};
    client_index = -1;
    last_packet_send_time = Time_Now() - Time_SecToTicks(60);
    last_packet_receive_time = Time_Now() - Time_SecToTicks(60);
}

void Client::Connect(Address address)
{
    Disconnect();
    server_address = address;
    state = CLIENT_SENDING_CONNECTION_REQUEST;
    last_packet_send_time = Time_Now() - Time_SecToTicks(1.0);
    last_packet_receive_time = Time_Now();
}

void Client::Disconnect()
{
    if (state == CLIENT_CONNECTED)
    {
        printf("client-side disconnect\n");
        ConnectionDisconnectPacket *packet = (ConnectionDisconnectPacket *)CreatePacket(PACKET_CONNECTION_DISCONNECT);
        SendPacketToServer(packet);
        DestroyPacket(packet);
    }

    ResetConnectionData();
}

void Client::SendPacketToServer(Packet *packet)
{
    ASSERT(packet);
    ASSERT(state != CLIENT_DISCONNECTED);
    ASSERT(AddressIsValid(server_address));
    SendPacket(socket, server_address, packet);
    last_packet_send_time = Time_Now();
}

void Client::SendPackets()
{
    double time_since_last_packet_sent = Time_Sec(Time_Since(last_packet_send_time));

    switch (state)
    {
        case CLIENT_SENDING_CONNECTION_REQUEST:
        {
            if (time_since_last_packet_sent < CONNECTION_REQUEST_SEND_RATE)
                return;

            char buffer[256];
            char *address_string = AddressToString(server_address, buffer, sizeof(buffer));
            printf("sending connection request to server: %s\n", address_string);

            ConnectionRequestPacket *packet = (ConnectionRequestPacket *)CreatePacket(PACKET_CONNECTION_REQUEST);
            SendPacketToServer(packet);
            DestroyPacket(packet);
        } break;

        case CLIENT_CONNECTED:
        {
            InputPacket *packet = (InputPacket *)CreatePacket(PACKET_INPUT);
            packet->input = game_data.input;
            SendPacketToServer(packet);
            DestroyPacket(packet);
        } break;

        default: break;
    }
}

void Client::ReceivePackets()
{
    while (true)
    {
        Address address;
        Packet *packet = ReceivePacket(socket, &address);
        if (!packet) break;

        switch (packet->type)
        {
            case PACKET_CONNECTION_ACCEPTED:
                ProcessConnectionAcceptedPacket((ConnectionAcceptedPacket *)packet, address);
                break;

            case PACKET_CONNECTION_DENIED:
                ProcessConnectionDeniedPacket((ConnectionDeniedPacket *)packet, address);
                break;

            case PACKET_CONNECTION_DISCONNECT:
                ProcessConnectionDisconnectPacket((ConnectionDisconnectPacket *)packet, address);
                break;

            case PACKET_CONNECTION_KEEP_ALIVE:
                ProcessConnectionKeepAlivePacket((ConnectionKeepAlivePacket *)packet, address);
                break;

            case PACKET_WORLD_STATE:
                ProcessWorldStatePacket((WorldStatePacket *)packet, address);
                break;

            default: break;
        }

        DestroyPacket(packet);
    }
}

void Client::CheckForTimeOut()
{
    double time_since_last_packet_received = Time_Sec(Time_Since(last_packet_receive_time));

    switch (state)
    {
        case CLIENT_SENDING_CONNECTION_REQUEST:
        {
            if (time_since_last_packet_received > CONNECTION_REQUEST_TIME_OUT)
            {
                printf("connection request to server timed out\n");
                state = CLIENT_ERROR;
                error = CLIENT_ERROR_CONNECTION_REQUEST_TIMED_OUT;
                return;
            }
        } break;

        case CLIENT_CONNECTED:
        {
            if (time_since_last_packet_received > CONNECTION_KEEP_ALIVE_TIME_OUT)
            {
                printf("keep alive timed out\n");
                state = CLIENT_ERROR;
                error = CLIENT_ERROR_KEEP_ALIVE_TIMED_OUT;
                Disconnect();
                return;
            }
        } break;

        default:
            break;
    }
}

void Client::ProcessConnectionAcceptedPacket(ConnectionAcceptedPacket *packet, Address address)
{
    if (state != CLIENT_SENDING_CONNECTION_REQUEST)
        return;

    if (!AddressCompare(address, server_address))
        return;

    char buffer[256];
    char *address_string = AddressToString(address, buffer, sizeof(buffer));
    printf("client is now connected to server: %s (client index = %d)\n", address_string, packet->client_index);
    state = CLIENT_CONNECTED;
    client_index = packet->client_index;
    last_packet_receive_time = Time_Now();
}

void Client::ProcessConnectionDeniedPacket(ConnectionDeniedPacket *packet, Address address)
{
    if (state != CLIENT_SENDING_CONNECTION_REQUEST)
        return;

    if (!AddressCompare(address, server_address))
        return;

    char buffer[256];
    char *address_string = AddressToString(address, buffer, sizeof(buffer));
    if (packet->reason == CONNECTION_DENIED_SERVER_FULL)
    {
        printf("client received connection denied from server: %s\n", address_string);
        state = CLIENT_ERROR;
        error = CLIENT_ERROR_UNKNOWN;
    }
}

void Client::ProcessConnectionDisconnectPacket(ConnectionDisconnectPacket *packet, Address address)
{
    if (state != CLIENT_CONNECTED)
        return;

    if (!AddressCompare(server_address, address))
        return;

    Disconnect();
}

void Client::ProcessConnectionKeepAlivePacket(ConnectionKeepAlivePacket *packet, Address address)
{
    if (state == CLIENT_DISCONNECTED)
        return;

    if (!AddressCompare(server_address, address))
        return;

    last_packet_receive_time = Time_Now();
}

void Client::ProcessWorldStatePacket(WorldStatePacket *packet, Address address)
{
    if (state != CLIENT_CONNECTED)
        return;

    if (!AddressCompare(server_address, address))
        return;

    ASSERT(packet->start_index >= 0);
    ASSERT(packet->start_index < Game::max_entity_count);
    ASSERT(packet->entity_count >= 0);
    ASSERT(packet->start_index + packet->entity_count <= Game::max_entity_count);

    int start_index = packet->start_index;
    int end_index = packet->start_index + packet->entity_count;
    for (int i = start_index; i < end_index; i++)
    {
        game_data.world.entities[i].type = packet->entities[i].type;
        game_data.world.entities[i].revision = packet->entities[i].revision;
        if (packet->entities[i].type != Game::ENTITY_TYPE_NONE)
        {
            game_data.world.entities[i].owner = packet->entities[i].owner;
            game_data.world.entities[i].current_frame = packet->entities[i].current_frame;
            game_data.world.entities[i].size = packet->entities[i].size;
            game_data.world.entities[i].position = packet->entities[i].position;
        }

    }

    last_packet_receive_time = Time_Now();
}

