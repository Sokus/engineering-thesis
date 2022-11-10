#include "address.h"

#include <stdio.h> // snprintf

namespace Net {

Address::Address()
{
    address = 0;
    port = 0;
}

Address::Address(unsigned int address, unsigned short port)
{
    this->address = address;
    this->port = port;
}

Address::Address(unsigned char a, unsigned char b,
                 unsigned char c, unsigned char d,
                 unsigned short port)
{
    this->address = ((a << 24) | (b << 16) | (c << 8) | (d));
    this->port = port;
}

unsigned int Address::GetAddress() const { return address; }
unsigned char Address::GetA() const { return (address >> 24 & 0xFF); }
unsigned char Address::GetB() const { return (address >> 16 & 0xFF); }
unsigned char Address::GetC() const { return (address >> 8 & 0xFF); }
unsigned char Address::GetD() const { return (address & 0xFF); }
unsigned short Address::GetPort() const { return port; }

void Address::SetAddress(unsigned int address)
{
    this->address = address;
}

void Address::SetPort(unsigned short port)
{
    this->port = port;
}

const char *Address::ToString()
{
    // xxx.xxx.xxx.xxx:xxxxx

    const int buffer_size = 22;
    static char buffer[buffer_size];
    snprintf(buffer, buffer_size,
             "%u.%u.%u.%u:%u",
             GetA(), GetB(), GetC(), GetD(), GetPort());
    return buffer;
}

bool AddressCompare(Address a, Address b)
{
    bool result = (a.GetAddress() == b.GetAddress() &&
                   a.GetPort() == b.GetPort());
    return result;
}

} // namespace Net