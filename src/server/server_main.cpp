#include <stdio.h>

#include "sockets.h"
#include "address.h"
#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "SERVER: Started");
    net::InitializeSockets();

    net::ShutdownSockets();
    Log(LOG_INFO, "SERVER: Exiting");
    return 0;
}