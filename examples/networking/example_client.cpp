#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"

#include <unistd.h>


int main(int argc, char *argv[])
{
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket(48700, false);
    Net::Address address = Net::Address(127, 0, 0, 1, 48620);

    Net::Channel channel = Net::Channel();
    channel.Bind(&socket, &address);

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        while(bytes_received = socket.Receive(nullptr, buffer, 4096))
        {
            channel.ReceivePacket(buffer, bytes_received);
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
        }

        channel.SendMessageCh(buffer, 1, true);

        channel.Update(1000.0f);
        channel.SendPackets();
        sleep(1);
        printf("\n");
    }

    Net::ShutdownSockets();

    return 0;
}