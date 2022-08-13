#include "config.h"

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <WS2tcpip.h>
#elif defined(PLATFORM_LINUX)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h> // close
    #include <errno.h> // errno
    #include <string.h> // strerr   or
#endif

#include "sockets.h"
#include "address.h"
#include "log.h"

namespace net {

bool InitializeSockets()
{
#if defined(PLATFORM_WINDOWS)
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2,2), &wsaData) == NO_ERROR;
#else
    return true;
#endif
}

void ShutdownSockets()
{
#if defined(PLATFORM_WINDOWS)
    WSACleanup();
#endif
}

int Socket::CreateHandle()
{
    int handle = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(handle <= 0)
    {
        handle = 0;
        Log(LOG_ERROR, "Could not create socket handle");
    }
    return handle;
}

bool Socket::Bind(unsigned short port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int bind_result = bind(handle, (struct sockaddr *)&address, sizeof(address));
    if(bind_result < 0)
    {
        Log(LOG_ERROR, "Could not bind socket");
        return false;
    }

    return true;
}

bool Socket::SetBlockingMode(bool should_block)
{
    bool success = false;
#if defined(PLATFORM_WINDOWS)
    u_long arg = !should_block;
    // TODO: See WSAGetLastError to get more detailed error codes
    success = ioctlsocket(handle, FIONBIO, &arg) == 0;
#elif defined(PLATFORM_LINUX)
    success = fcntl(handle, F_SETFL, O_NONBLOCK, !should_block) != -1;
#endif
    if(!success)
        Log(LOG_ERROR, "Could not set socket blocking mode");

    return success;
}

Socket::Socket()
{
    Log(LOG_DEBUG, "Creating socket");
    handle = CreateHandle();
    SetBlockingMode(false);
}

Socket::Socket(bool should_block)
{
    Log(LOG_DEBUG, "Creating socket");
    handle = CreateHandle();
    SetBlockingMode(should_block);
}

Socket::Socket(unsigned short port)
{
    Log(LOG_DEBUG, "Creating socket");
    handle = CreateHandle();
    SetBlockingMode(false);
    Bind(port);
}

Socket::Socket(unsigned short port, bool should_block)
{
    Log(LOG_DEBUG, "Creating socket");
    this->handle = CreateHandle();
    SetBlockingMode(should_block);
    Bind(port);
}

Socket::~Socket()
{
    Log(LOG_DEBUG, "Destroying socket");
#if defined(PLATFORM_WINDOWS)
    closesocket(handle);
#elif defined(PLATFORM_LINUX)
    close(handle);
#endif
}

bool Socket::Send(const Address& destination, const void *data, int size)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(destination.GetAddress());
    addr.sin_port = htons(destination.GetPort());

    struct sockaddr *addr_ptr = (struct sockaddr *)&addr;
    long int bytes_sent = sendto(handle, (const char *)data, (size_t)size, 0, addr_ptr, sizeof(addr));

    if(bytes_sent != size)
    {
        Log(LOG_WARNING, "Could not send packet");
        return false;
    }

    return true;
}

int Socket::Receive(Address *sender, void *data, int size)
{
    struct sockaddr_in from;
    socklen_t from_length = sizeof(from);

    struct sockaddr *from_ptr = (struct sockaddr *)&from;
    int bytes_recieved = (int)recvfrom(handle, (char *)data, (size_t)size, 0, from_ptr, &from_length);

    if(bytes_recieved == -1 && errno != EWOULDBLOCK)
        Log(LOG_ERROR, "Socket::Receive: %s", strerror(errno));

    if(sender != nullptr)
        *sender = Address(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));

    return bytes_recieved;
}

} // namespace net
