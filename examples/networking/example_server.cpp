#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "pi_time.h"
#include "config.h"


#include <stddef.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    Net::InitializeSockets();
    Time::Setup();

    Net::Socket socket = Net::Socket(48620, false);

    Net::Channel channel = Net::Channel();

    Net::Address client;
    bool received_anything = false;

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        Net::Address sender;
        while(bytes_received = socket.Receive(&sender, buffer, 4096))
        {
            if(received_anything == false)
            {
                client = sender;
                channel.Bind(&socket, &client);
                received_anything = true;
            }

            channel.ReceivePacket(buffer, bytes_received);
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
            printf("Message received: %u\n", *(uint32_t *)buffer);
        }

        Time::SleepMs(300);

        if(received_anything)
        {
            int16_t data = -13;
            channel.SendMessageEx(&data, sizeof(data), false);
        }
        channel.SendPackets();
        channel.NextFrame();
    }

    Net::ShutdownSockets();

    return 0;
}