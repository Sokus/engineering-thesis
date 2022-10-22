#include <stdint.h>
#include <stdio.h>

#include "networking/networking.h"
#include "serialization/serialization.h"
#include "log.h"
#include "Windows.h"


int main(int argc, char *argv[])
{
    Net::InitializeSockets();

    Net::Socket socket = Net::Socket(48700, false);
    Net::Address server = Net::Address(127, 0, 0, 1, 48620);
    Net::Channel channel = Net::Channel();
    channel.Bind(&socket, &server);

    int counter = 0;

    while(true)
    {
        uint8_t in_buf[2048];
        Net::Address sender;
        int bytes_received;
        while(bytes_received = socket.Receive(&sender, in_buf, sizeof(in_buf)))
        {
            channel.Receive(in_buf, bytes_received);
        }

        Sleep(1000);

        int data_size;
        void *data;
        while(data = channel.GetData(&data_size))
        {
            printf("Got %u bytes of data: %i\n", data_size, *((int*)data));
        }

        channel.Send(&counter, sizeof(counter));

        printf("Counter: %d\n", counter++);
    }

    Net::ShutdownSockets();

    return 0;
}