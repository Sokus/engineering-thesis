#include "address.h"
#include "sockets.h"
#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Client starting");
    net::InitializeSockets();

    net::Socket s = net::Socket();

    net::Address server_address = net::Address(127, 0, 0, 1, 25565);

    for(int i = 1; i < 6; ++i)
    {
        int data = i;
        Log(LOG_INFO, "Sending %d", data);
        s.Send(server_address, &data, sizeof(data));
    }

    Log(LOG_INFO, "Client shutting down");
    net::ShutdownSockets();
    return 0;
}