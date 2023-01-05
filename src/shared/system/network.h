#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool InitializeNetwork();
void ShutdownNetwork();
bool IsNetworkInitialized();

typedef enum AddressType
{
    ADDRESS_UNDEFINED,
    ADDRESS_IPV4,
    ADDRESS_IPV6
} AddressType;

typedef struct Address
{
    AddressType type;

    union {
        uint32_t ipv4;
        uint16_t ipv6[8];
    };

    uint16_t port;
} Address;

Address Address4(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);
Address Address4FromInt(uint32_t address, uint16_t port);
Address Address6(uint16_t a, uint16_t b, uint16_t c, uint16_t d,
                 uint16_t e, uint16_t f, uint16_t g, uint16_t h,
                 uint16_t port);
Address Address6FromArray(uint16_t address[], uint16_t port);
struct sockaddr_storage;
Address AddressFromSockaddr(struct sockaddr_storage *addr);
Address AddressParse(char *address_in);
Address AddressParseEx(char *address_in, uint16_t port);
char *AddressToString(Address address, char buffer[], int buffer_size);
bool AddressIsValid(Address address);
bool AddressCompare(Address a, Address b);

#if _WIN32
typedef uint64_t SocketHandle;
#else
typedef int SocketHandle;
#endif

typedef enum SocketType
{
    SOCKET_UNDEFINED,
    SOCKET_IPV4,
    SOCKET_IPV6
} SocketType;

typedef enum SocketError
{
    SOCKET_ERROR_NONE,
    SOCKET_ERROR_CREATE_FAILED,
    SOCKET_ERROR_SET_NON_BLOCKING_FAILED,
    SOCKET_ERROR_SOCKOPT_IPV6_ONLY_FAILED,
    SOCKET_ERROR_BIND_IPV4_FAILED,
    SOCKET_ERROR_BIND_IPV6_FAILED,
    SOCKET_ERROR_GET_SOCKNAME_IPV4_FAILED,
    SOCKET_ERROR_GET_SOCKNAME_IPV6_FAILED
} SocketError;

typedef struct Socket
{
    SocketHandle handle;
    uint16_t port;
    int error;
} Socket;

Socket SocketCreate(SocketType type, uint16_t port);
void SocketDestroy(Socket *socket);
bool SocketSend(Socket socket, Address address, void *packet_data, size_t packet_bytes);
int SocketReceive(Socket socket, Address *from, void *packet_data, int max_packet_size);
bool SocketIsError(Socket socket);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_H