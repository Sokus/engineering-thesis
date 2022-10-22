#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "Windows.h"

#include <stddef.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket(48620, false);
    Net::Channel channel = Net::Channel();

    bool got_anything = false;

    int counter = 0;

    while(true)
    {
        uint8_t in_buf[2048];
        Net::Address sender;
        int bytes_received;
        while(bytes_received = socket.Receive(&sender, in_buf, sizeof(in_buf)))
        {
            if(got_anything == false)
            {
                static Net::Address sender_static = sender;
                channel.Bind(&socket, &sender_static);
                got_anything = true;
            }

            channel.Receive(in_buf, bytes_received);
        }

        Sleep(2000);

        int data_size;
        void *data;
        while(data = channel.GetData(&data_size))
        {
            printf("Got %u bytes of data: %i\n", data_size, *((int*)data));
        }

        if(got_anything)
        {
            channel.Send(&counter, sizeof(counter));
        }

        printf("Counter: %d\n", counter++);
    }

    Net::ShutdownSockets();

    return 0;
}