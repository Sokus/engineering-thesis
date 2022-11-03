#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"

#include "config.h"

#if defined(PLATFORM_LINUX)
    #include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
    #include <Windows.h>
#endif

#include <stddef.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket(48620, false);

    Net::Channel channel = Net::Channel();

    Net::Address address;
    Net::Address address2;
    bool received_anything = false;

    while(true)
    {
        uint8_t buffer[4096];
        int bytes_received;
        while(bytes_received = socket.Receive(&address, buffer, 4096))
        {
            if(received_anything == false)
            {
                address2 = address;
                channel.Bind(&socket, &address2);
                received_anything = true;
            }

            channel.ReceivePacket(buffer, bytes_received);
        }

        while(channel.ReceiveMessage(buffer, &bytes_received))
        {
        }


        #if defined(PLATFORM_LINUX)
            sleep(3);
        #elif defined(PLATFORM_WINDOWS)
            Sleep(3000);
        #endif

        printf("\n");

        if(received_anything)
        {
            channel.SendMessageCh(buffer, 1, true);
        }
        channel.Update(3000.0f);
        channel.SendPackets();

    }

    Net::ShutdownSockets();

    return 0;
}