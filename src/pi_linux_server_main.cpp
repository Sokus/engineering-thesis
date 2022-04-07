#include <stdio.h>
#include <stdint.h>

// networking
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_addr
#include <fcntl.h>        // set file/socket handle to non-blocking
#include <unistd.h>       // write(), close()
#include <errno.h>        // socket error handling

#include "pi_helpers.h"
#include "pi_networking.h"
#include "pi_linux_networking.cpp"
#include "pi_networking.cpp"

int main(void)
{
    int socket_handle;
    if(!SocketCreate(&socket_handle))
        return -1;
    
    if(!SocketBind(socket_handle, 5000))
        return -1;
    
    uint8_t buffer_in[2048];
    
    MemoryArena buffer_in_arena;
    InitializeArena(&buffer_in_arena, buffer_in, sizeof(buffer_in));
    
    while(true)
    {
        Address sender;
        int bytes_read = SocketRecieve(socket_handle, &sender,
                                       buffer_in_arena.base,
                                       (int)buffer_in_arena.size);
        
        AddressComponents sender_componentwise = GetAddressComponents(sender);
        printf("%d bytes from %u.%u.%u.%u:%u\n",
               bytes_read,
               sender_componentwise.a,
               sender_componentwise.b,
               sender_componentwise.c,
               sender_componentwise.d,
               sender_componentwise.port);
    }
    
    return 0;
}