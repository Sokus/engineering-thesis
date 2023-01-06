#include "server.h"

#include <string.h>

#include "macros.h"
#include "system/pi_time.h"

void Server::Init(Socket socket)
{
    memset(this, 0, sizeof(Server));
    this->socket = socket;
    for (int i = 0; i < MAX_CLIENTS; i++)
        ResetClientState(i);
}

void Server::ResetClientState(int client_index)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    client_connected[client_index] = false;
    client_address[client_index] = Address{};
    client_data[client_index] = ServerClientData{};
}

void Server::SendPackets()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
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

            default: break;
        }

        DestroyPacket(packet);
    }
}

void Server::SendPacketToConnectedClient(int client_index, Packet *packet)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    ASSERT(packet);
    ASSERT(client_connected[client_index]);
    SendPacket(socket, client_address[client_index], packet);
    client_data[client_index].last_packet_send_time = Time_Now();
}

int Server::FindFreeClientIndex()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!client_connected[i])
            return i;
    }
    return -1;
}

int Server::FindExistingClientIndex(Address address)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_connected[i] && AddressCompare(client_address[i], address))
            return i;
    }
    return -1;
}

void Server::ConnectClient(int client_index, Address address)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    ASSERT(num_connected_clients >= 0);
    ASSERT(num_connected_clients < MAX_CLIENTS - 1);
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
}

void Server::DisconnectClient(int client_index)
{
    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);
    ASSERT(num_connected_clients > 0);
    ASSERT(client_connected[client_index]);

    char buffer[256];
    char *address_string = AddressToString(client_address[client_index], buffer, sizeof(buffer));
    printf("client %d disconnected (address = %s)\n", client_index, address_string);

    ConnectionDisconnectPacket *packet = (ConnectionDisconnectPacket *)CreatePacket(PACKET_CONNECTION_DISCONNECT);
    SendPacketToConnectedClient(client_index, packet);
    DestroyPacket(packet);

    ResetClientState(client_index);
    num_connected_clients--;
}

void Server::CheckForTimeOut()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
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

void Server::ProcessConnectionRequestPacket(ConnectionRequestPacket *packet, Address address)
{
    int existing_client_index = FindExistingClientIndex(address);
    if (existing_client_index != -1)
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

    ASSERT(client_index != -1);
    if (client_index == -1)
        return;

    ConnectClient(client_index, address);
}

void Server::ProcessConnectionDisconnectPacket(ConnectionDisconnectPacket *packet, Address address)
{
    int client_index = FindExistingClientIndex(address);
    if (client_index == -1)
        return;

    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);


    DisconnectClient(client_index);
}

void Server::ProcessConnectionKeepAlivePacket(ConnectionKeepAlivePacket *packet, Address address)
{
    int client_index = FindExistingClientIndex(address);
    if (client_index == -1)
        return;

    ASSERT(client_index >= 0);
    ASSERT(client_index < MAX_CLIENTS);

    client_data[client_index].last_packet_receive_time = Time_Now();
}
