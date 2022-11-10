#ifndef PI_ADDRESS_H
#define PI_ADDRESS_H

namespace Net {

class Address
{
private:
    unsigned int address;
    unsigned short port;

public:
    Address();
    Address(unsigned char a,
            unsigned char b,
            unsigned char c,
            unsigned char d,
            unsigned short port);
    Address(unsigned int address, unsigned short port);

    unsigned int GetAddress() const;
    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;

    unsigned short GetPort() const;

    void SetAddress(unsigned int address);
    void SetPort(unsigned short port);

    const char *ToString();
};

bool AddressCompare(Address a, Address b);

} // namespace Net

#endif // PI_ADDRESS_H