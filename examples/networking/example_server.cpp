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
    Log(LOG_INFO, "Server starting");
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket(25565, true);
    Log(LOG_INFO, "Waiting for a packet");
    unsigned char data[4096];
    Net::Address sender;
    int data_size = socket.Receive(&sender, data, 4096);

    bool ib1;
    bool ib2;
    bool ib3;
    BitPacker br = BitReader(data, sizeof(data));
    SERIALIZE_BOOL(&br, &ib1);
    SERIALIZE_BOOL(&br, &ib2);
    SERIALIZE_BOOL(&br, &ib3);

    Log(LOG_INFO,
        "Booleans received: %s %s %s",
        StringifyBool(ib1),
        StringifyBool(ib2),
        StringifyBool(ib3)
    );

    Log(LOG_INFO, "Server shutting down");
    Net::ShutdownSockets();
    return 0;
}