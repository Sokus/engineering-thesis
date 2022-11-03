#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "networking.h"

#include "data_structures.h"

#include <stdint.h>

namespace Net {

struct PacketHeader
{
};

class Channel
{
private:
    Socket *socket = nullptr;
    Address *address = nullptr;

public:
    Channel();

    void Bind(Socket *socket, Address *address);
    void Update(float dt);
    void SendPackets();
    void ReceivePacket(void *data, int size);

    void SendMessageCh(void *data, int size, bool ordered);

    bool ReceiveMessage(void *data, int *size);
};

inline bool SequenceGreaterThan(uint16_t s1, uint16_t s2);

} // namespace Net

#endif // RELIABILITY_H