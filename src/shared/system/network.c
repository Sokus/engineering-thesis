#include "network.h"
#include "macros.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
	#define NOMINMAX
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <ws2ipdef.h>
	#pragma comment( lib, "WS2_32.lib" )

	#ifdef SetPort
	#undef SetPort
	#endif // #ifdef SetPort
#else
	#include <netdb.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
#endif

static bool network_initialized = false;

bool InitializeNetwork()
{
    ASSERT(!network_initialized);
    bool result = true;
#if _WIN32
    WSADATA WsaData;
    result = WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#endif
    if (result)
        network_initialized = result;
    return result;
}

void ShutdownNetwork()
{
    ASSERT(network_initialized);
#if _WIN32
    WSACleanup();
#endif
    network_initialized = false;
}

bool IsNetworkInitialized()
{
    return network_initialized;
}

Address Address4(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port)
{
    Address result = {0};
    result.type = ADDRESS_IPV4;
    result.ipv4 = (uint32_t)a | ((uint32_t)b << 8) | ((uint32_t)c << 16) | ((uint32_t)d << 24);
    result.port = port;
    return result;
}

Address Address4FromInt(uint32_t address, uint16_t port)
{
    Address result = {0};
    result.type = ADDRESS_IPV4;
    result.ipv4 = htonl(address);
    result.port = port;
    return result;
}

Address Address6(uint16_t a, uint16_t b, uint16_t c, uint16_t d,
                 uint16_t e, uint16_t f, uint16_t g, uint16_t h,
                 uint16_t port)
{
    Address result = {0};
    result.type = ADDRESS_IPV6;
    result.ipv6[0] = htons(a);
    result.ipv6[1] = htons(b);
    result.ipv6[2] = htons(c);
    result.ipv6[3] = htons(d);
    result.ipv6[4] = htons(e);
    result.ipv6[5] = htons(f);
    result.ipv6[6] = htons(g);
    result.ipv6[7] = htons(h);
    result.port = port;
    return result;
}

Address Address6FromArray(uint16_t address[], uint16_t port)
{
    Address result = {0};
    result.type = ADDRESS_IPV6;
    for (int i = 0; i < 8; i++)
        result.ipv6[i] = htons(address[i]);
    result.port = port;
    return result;
}

Address AddressFromSockaddr(struct sockaddr_storage *addr)
{
    Address result = {0};

    ASSERT(addr->ss_family == AF_INET || addr->ss_family == AF_INET6);
    if (addr->ss_family == AF_INET)
    {
        struct sockaddr_in *addr_ipv4 = (struct sockaddr_in *)addr;
        result.type = ADDRESS_IPV4;
        result.ipv4 = addr_ipv4->sin_addr.s_addr;
        result.port = ntohs(addr_ipv4->sin_port);
    }
    else if (addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *addr_ipv6 = (struct sockaddr_in6 *)addr;
        result.type = ADDRESS_IPV6;
        memcpy(result.ipv6, &addr_ipv6->sin6_addr, 16);
        result.port = ntohs(addr_ipv6->sin6_port);
    }

    return result;
}

Address AddressParse(char *address_in)
{
    ASSERT(address_in);
    Address result = {0};

    char buffer[256];
    char *address = buffer;
    strncpy(address, address_in, 255);
    address[255] = '\0';

    // first try to parse as an IPv6 address:
    // 1. if the first character is '[' then it's probably an ipv6 in form "[addr6]:portnum"
    // 2. otherwise try to parse as raw IPv6 address, parse using inet_pton
    int address_length = (int)strlen(address);
    if (address[0] == '[')
    {
        const int base_index = address_length - 1;
        for (int i = 0; i < 6; ++i) // note: no need to search past 6 characters as ":65535" is longest port value
        {
            const int index = base_index - i;
            if (index < 3)
                break;
            if (address[index] == ':')
            {
                result.port = (uint16_t)atoi(&address[index + 1]);
                address[index - 1] = '\0';
            }
        }
        address += 1;
    }
    struct in6_addr sockaddr6;
    if (inet_pton(AF_INET6, address, &sockaddr6) == 1)
    {
        result.type = ADDRESS_IPV6;
        memcpy(result.ipv6, &sockaddr6, 16);
        return result;
    }

    // otherwise it's probably an IPv4 address:
    // 1. look for ":portnum", if found save the portnum and strip it out
    // 2. parse remaining ipv4 address via inet_pton
    address_length = (int)strlen(address);
    const int base_index = address_length - 1;
    for (int i = 0; i < 6; ++i) // note: no need to search past 6 characters as ":65535" is longest port value
    {
        const int index = base_index - i;
        if (index < 0)
            break;
        if (address[index] == ':')
        {
            result.port = (uint16_t)atoi(&address[index + 1]);
            address[index] = '\0';
        }
    }

    struct sockaddr_in sockaddr4;
    if (inet_pton(AF_INET, address, &sockaddr4.sin_addr) == 1)
    {
        result.type  = ADDRESS_IPV4;
        result.ipv4 = sockaddr4.sin_addr.s_addr;
    }
    else
    {
        // nope: it's not an IPv4 address. maybe it's a hostname? set address as undefined.
        result = (Address){0};
    }

    return result;
}

Address AddressParseEx(char *address_in, uint16_t port)
{
    Address result = AddressParse(address_in);
    result.port = port;
    return result;
}

char *AddressToString(Address address, char buffer[], int buffer_size)
{
    if (address.type == ADDRESS_IPV4)
    {
        uint8_t a = address.ipv4 & 0xFF;
        uint8_t b = (address.ipv4 >> 8) & 0xFF;
        uint8_t c = (address.ipv4 >> 16) & 0xFF;
        uint8_t d = (address.ipv4 >> 24) & 0xFF;
        if (address.port != 0)
            snprintf(buffer, buffer_size, "%u.%u.%u.%u:%u", a, b, c, d, address.port);
        else
            snprintf(buffer, buffer_size, "%u.%u.%u.%u", a, b, c, d);
        return buffer;
    }
    else if (address.type == ADDRESS_IPV6)
    {
        if (address.port == 0)
        {
            inet_ntop(AF_INET6, (void *)&address.ipv6, buffer, buffer_size);
            return buffer;
        }
        else
        {
            char address_string[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, (void *)&address.ipv6, address_string, INET6_ADDRSTRLEN);
            snprintf(buffer, buffer_size, "[%s]:%u", address_string, address.port);
            return buffer;
        }
    }
    else
    {
        return "undefined";
    }
}

bool AddressIsValid(Address address)
{
    return address.type != ADDRESS_UNDEFINED;
}

bool AddressCompare(Address a, Address b)
{
    if (a.port != b.port)
        return false;
    if (a.type == ADDRESS_IPV4 && b.type == ADDRESS_IPV4)
    {
        if (a.ipv4 == b.ipv4)
            return true;
    }
    if (a.type == ADDRESS_IPV6 && b.type == ADDRESS_IPV6)
    {
        if (memcmp(a.ipv6, b.ipv6, sizeof(a.ipv6)) == 0)
            return true;
    }
    return false;
}

Socket SocketCreate(SocketType type, uint16_t port)
{
    ASSERT(IsNetworkInitialized());
    Socket result = {0};

    // Create socket
    result.handle = socket((type == SOCKET_IPV6) ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (result.handle <= 0)
    {
        result.error = SOCKET_ERROR_CREATE_FAILED;
        return result;
    }

    // Force IPv6 if necessary
    if (type == SOCKET_IPV6)
    {
        char optval = 1;
        if (setsockopt(result.handle, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval)) != 0)
        {
            printf("failed to set ipv6 only sockopt\n");
            result.error = SOCKET_ERROR_SOCKOPT_IPV6_ONLY_FAILED;
            return result;
        }
    }

    // Bind to port
    if (type == SOCKET_IPV6)
    {
        struct sockaddr_in6 sock_address = {0};
        sock_address.sin6_family = AF_INET6;
        sock_address.sin6_addr = in6addr_any;
        sock_address.sin6_port = htons(port);

        if (bind(result.handle, (const struct sockaddr *)&sock_address, sizeof(sock_address)) < 0)
        {
            result.error = SOCKET_ERROR_BIND_IPV6_FAILED;
            return result;
        }
    }
    else
    {
        struct sockaddr_in sock_address;
        sock_address.sin_family = AF_INET;
        sock_address.sin_addr.s_addr = INADDR_ANY;
        sock_address.sin_port = htons(port);

        if (bind(result.handle, (const struct sockaddr *)&sock_address, sizeof(sock_address)) < 0)
        {
            result.error = SOCKET_ERROR_BIND_IPV4_FAILED;
            return result;
        }
    }

    // If bound to port 0 find the actual port we got
    result.port = port;
    if (result.port == 0)
    {
        if (type == SOCKET_IPV6)
        {
            struct sockaddr_in6 sin;
            socklen_t len = sizeof(sin);
            if (getsockname(result.handle, (struct sockaddr *)&sin, &len) == -1)
            {
                result.error = SOCKET_ERROR_GET_SOCKNAME_IPV6_FAILED;
                return result;
            }
            result.port = ntohs(sin.sin6_port);
        }
        else if (type == SOCKET_IPV4)
        {
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(result.handle, (struct sockaddr *)&sin, &len) == -1)
            {
                result.error = SOCKET_ERROR_GET_SOCKNAME_IPV4_FAILED;
                return result;
            }
            result.port = ntohs(sin.sin_port);
        }
    }

    // Set non-blocking IO
#if _WIN32
    DWORD non_blocking = 1;
    if (ioctlsocket(result.handle, FIONBIO, &non_blocking) != 0)
    {
        printf("failed to make socket non-blocking\n");
        result.error = SOCKET_ERROR_SET_NON_BLOCKING_FAILED;
        return result;
    }
#else
    int non_blocking = 1;
    if (fcntl(result.handle, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to make socket non-blocking\n");
        result.error = SOCKET_ERROR_SET_NON_BLOCKING_FAILED;
        return result;
    }
#endif

    return result;
}

void SocketDestroy(Socket *socket)
{
    if (socket->handle != 0)
    {
#if _WIN32
        closesocket(socket->handle);
#else
        close(socket->handle);
#endif
        socket->handle = 0;
    }
}

bool SocketSend(Socket socket, Address address, void *packet_data, size_t packet_bytes)
{
    ASSERT(packet_data);
    ASSERT(packet_bytes > 0);
    ASSERT(AddressIsValid(address));
    ASSERT(socket.handle);
    ASSERT(!SocketIsError(socket));

    bool result = false;

    if (address.type == ADDRESS_IPV6)
    {
        struct sockaddr_in6 socket_address = {0};
        socket_address.sin6_family = AF_INET6;
        socket_address.sin6_port = htons(address.port);
        memcpy(&socket_address.sin6_addr, address.ipv6, sizeof(socket_address.sin6_addr));
        size_t sent_bytes = sendto(socket.handle, packet_data, (int)packet_bytes, 0, (struct sockaddr *)&socket_address, sizeof(socket_address));
        result = (sent_bytes == packet_bytes);
    }
    else if (address.type == ADDRESS_IPV4)
    {
        struct sockaddr_in socket_address = {0};
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = address.ipv4;
        socket_address.sin_port = htons(address.port);
        size_t sent_bytes = sendto(socket.handle, packet_data, (int)packet_bytes, 0, (struct sockaddr *)&socket_address, sizeof(socket_address));
        result = (sent_bytes == packet_bytes);
    }

    return result;
}

int SocketReceive(Socket socket, Address *from, void *packet_data, int max_packet_size)
{
    ASSERT(socket.handle);
    ASSERT(packet_data);
    ASSERT(max_packet_size > 0);

#if _WIN32
    typedef int socklen_t;
#endif

    struct sockaddr_storage sockaddr_from;
    socklen_t from_length = sizeof(sockaddr_from);
    int result = recvfrom(socket.handle, packet_data, max_packet_size, 0, (struct sockaddr *)&sockaddr_from, &from_length);

#if _WIN32
    if (result == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        // TODO: Check if it is safe to ignore WSAECONNRESET
        if (error == WSAEWOULDBLOCK || error == WSAECONNRESET)
            return 0;
        printf("recvfrom failed: %d\n", error);
        return 0;
    }
#else
    if (result <= 0)
    {
        if (errno == EAGAIN)
            return 0;
        printf("recvfrom failed: %s\n", strerror(errno));
        return 0;
    }
#endif

    ASSERT(result >= 0);
    *from = AddressFromSockaddr(&sockaddr_from);
    return result;
}

bool SocketIsError(Socket socket)
{
    return socket.error != SOCKET_ERROR_NONE;
}