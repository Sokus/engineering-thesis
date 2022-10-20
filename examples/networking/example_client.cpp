#include <stdint.h>
#include <stdio.h>

#include "networking/address.h"
#include "networking/sockets.h"
#include "serialization/serialize.h"
#include "serialization/bitpacker.h"
#include "log.h"

const char *StringifyBool(bool value)
{
    switch(value)
    {
        case false: return "false";
        case true:  return "true";
    }

    return "???";
}

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "Client starting");
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket();

    bool ob1 = true;
    bool ob2 = false;
    bool ob3 = true;

    char buffer[128];
    BitPacker bw = BitWriter(buffer, sizeof(buffer));
    SERIALIZE_BOOL(&bw, &ob1);
    SERIALIZE_BOOL(&bw, &ob2);
    SERIALIZE_BOOL(&bw, &ob3);
    Flush(&bw);

    Log(LOG_INFO,
        "Sending three booleans: %s %s %s",
        StringifyBool(ob1),
        StringifyBool(ob2),
        StringifyBool(ob3)
    );

    Net::Address server = Net::Address(127, 0, 0, 1, 25565);
    socket.Send(server, buffer, BytesWritten(&bw));

    Log(LOG_INFO, "Client shutting down");
    Net::ShutdownSockets();

    return 0;
}