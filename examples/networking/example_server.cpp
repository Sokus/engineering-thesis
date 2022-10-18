#include "networking/address.h"
#include "networking/sockets.h"
#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Server starting");
    Net::InitializeSockets();

    Log(LOG_INFO, "Server shutting down");
    Net::ShutdownSockets();
    return 0;
}