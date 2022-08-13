#include "address.h"

namespace net {

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

bool AddressCompare(Address a, Address b)
{
    bool result = (a.GetAddress() == b.GetAddress() &&
                   a.GetPort() == b.GetPort());
    return result;
}

} // namespace net