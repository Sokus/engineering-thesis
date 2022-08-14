#include "address.h"
#include "sockets.h"
#include "log.h"
#include "dns.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Client starting");
    net::InitializeSockets();

    const char *hostname = "google.com";
    net::Address address;
    address.SetAddress(net::QueryDNS(hostname));

    if(address.GetAddress() != 0)
    {
        Log(LOG_INFO, "%s can be found on %d.%d.%d.%d",
            hostname, address.GetA(), address.GetB(), address.GetC(), address.GetD());
    }

    Log(LOG_INFO, "Client shutting down");
    net::ShutdownSockets();
    return 0;
}