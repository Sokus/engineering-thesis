#include "address.h"
#include "sockets.h"
#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Server starting");
    net::InitializeSockets();

    net::Socket s = net::Socket((unsigned short)25565, true);

    while(true)
    {
        net::Address sender;
        int incoming_data;
        s.Receive(&sender, &incoming_data, sizeof(incoming_data));
        Log(LOG_INFO, "Received %d from %d.%d.%d.%d:%d",
            incoming_data, sender.GetA(), sender.GetB(),
            sender.GetC(), sender.GetD(), sender.GetPort());
        if(incoming_data == 5)
            break;
    }

    Log(LOG_INFO, "Server shutting down");
    net::ShutdownSockets();
    return 0;
}