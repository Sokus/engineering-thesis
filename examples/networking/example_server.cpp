#include "address.h"
#include "sockets.h"
#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Server starting");
    net::InitializeSockets();

    Log(LOG_INFO, "Server shutting down");
    net::ShutdownSockets();
    return 0;
}