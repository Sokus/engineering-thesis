#ifndef PI_SOCKETS_H
#define PI_SOCKETS_H

#include <stdint.h>

#include "address.h"

namespace Net {

class Socket
{
private:
    int handle;
    bool is_open;

    int CreateHandle();
    bool Bind(unsigned short port);
    bool SetBlockingMode(bool should_block);
public:
    Socket();
    Socket(bool should_block);
    Socket(unsigned short port);
    Socket(unsigned short port, bool should_block);
    ~Socket();
    bool Send(const Address& destination, const void *data, int size);
    int Receive(Address *sender, void *data, int size);
};

bool InitializeSockets();
void ShutdownSockets();

} // namespace Net

#endif // PI_SOCKETS_H