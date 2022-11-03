#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"

#include "config.h"

#if defined(PLATFORM_LINUX)
    #include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
    #include <Windows.h>
#endif


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


        #if defined(PLATFORM_LINUX)
            sleep(1);
        #elif defined(PLATFORM_WINDOWS)
            Sleep(1000);
        #endif

        printf("\n");

        channel.SendMessageCh(buffer, 1, true);

        channel.Update(1000.0f);
        channel.SendPackets();

    }

    Net::ShutdownSockets();

    return 0;
}