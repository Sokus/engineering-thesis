bool SocketsInit()
{
    return true;
}

void SocketsShutdown()
{
    return;
}

bool SocketCreate(int *socket_handle)
{
    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(handle <= 0)
    {
        fprintf(stderr, "ERROR: Could not create socket\n");
        INVALID_CODE_PATH;
        return false;
    }
    
    *socket_handle = handle;
    return true;
}

bool SocketSetShouldBlock(int socket_handle, bool should_block)
{
    int fcntl_result = fcntl(socket_handle, F_SETFL, O_NONBLOCK, should_block);
    
    if(fcntl_result == -1)
    {
        char *message = "ERROR: Could not set socket blocking mode\n";
        fprintf(stderr, message);
        INVALID_CODE_PATH;
        return false;
    }
    
    return true;
}

bool SocketBind(int socket_handle, uint16_t port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    int bind_result = bind(socket_handle,
                           (struct sockaddr *)&address,
                           sizeof(address));
    if(bind_result < 0)
    {
        fprintf(stderr, "[ERROR] Bind socket\n");
        INVALID_CODE_PATH;
        return false;
    }
    
    return true;
}

bool SocketClose(int socket_handle)
{
    int rc = close(socket_handle);
    bool success = (rc == 0);
    return success;
}

bool SocketSend(int socket_handle, Address *destination, void *data, int size)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(destination->ip);
    addr.sin_port = htons(destination->port);
    
    struct sockaddr *addr_ptr = (struct sockaddr *)&addr;
    long int bytes_sent = sendto(socket_handle, data, (size_t)size, 0, addr_ptr, sizeof(addr));
    
    if(bytes_sent != size)
    {
        fprintf(stderr, "ERROR: Failed to send packet\n");
        return false;
    }
    
    return true;
}

int SocketRecieve(int socket_handle, Address *sender, void *data, int size)
{
    struct sockaddr_in from;
    socklen_t from_length = sizeof(from);
    
    struct sockaddr *from_ptr = (struct sockaddr *)&from;
    int bytes_recieved = (int)recvfrom(socket_handle, data, (size_t)size, 0, from_ptr, &from_length);
    
    if(bytes_recieved == -1 && errno != EWOULDBLOCK)
    {
        fprintf(stderr, "[ERROR] SocketRecieve: %s\n", strerror(errno));
        INVALID_CODE_PATH;
    }
    
    if(sender != 0)
    {
        sender->ip = ntohl(from.sin_addr.s_addr);
        sender->port = ntohs(from.sin_port);
    }
    
    return bytes_recieved;
}
