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

    {
        uint32_t data;
        data = 1337;
        channel.SendMessageEx(&data, sizeof(uint32_t), false);
        channel.SendPackets();
            channel.NextFrame();
    }

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        Net::Address sender;
        bool received_anything = false;
        while(bytes_received = socket.Receive(&sender, buffer, 4096))
        {
            ASSERT(sender.GetAddress() == address.GetAddress() &&
                   sender.GetPort() == address.GetPort());

            channel.ReceivePacket(buffer, bytes_received);
            received_anything = true;
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
            printf("Message received: %d\n", *(int16_t *)buffer);
            received_anything = true;
        }

        Time::SleepMs(15);

        if(received_anything)
        {
            uint32_t data;
            data = 1337;

            channel.SendMessageEx(&data, sizeof(uint32_t), false);
            channel.SendPackets();
            channel.NextFrame();

            data = 7331;

            channel.SendMessageEx(&data, sizeof(uint32_t), false);
            channel.SendPackets();
            channel.NextFrame();
        }
    }

    Net::ShutdownSockets();

    return 0;
}