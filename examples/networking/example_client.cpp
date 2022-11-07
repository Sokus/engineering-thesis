#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "config.h"

int main(int argc, char *argv[])
{
    Net::InitializeSockets();
    Time::Setup();

    Net::Socket socket = Net::Socket(48700, false);
    Net::Address address = Net::Address(127, 0, 0, 1, 48620);

    Net::Channel channel = Net::Channel();
    channel.Bind(&socket, &address);

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        Net::Address sender;
        while(bytes_received = socket.Receive(&sender, buffer, 4096))
        {
            channel.ReceivePacket(buffer, bytes_received);
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
            printf("Message received: %d\n", *(int16_t *)buffer);
        }

        Time::SleepMs(100);

        uint32_t data = 1337;

        channel.SendMessageEx(&data, sizeof(uint32_t), false);
        channel.SendPackets();
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}