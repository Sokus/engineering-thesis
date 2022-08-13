#ifndef PI_DNS_H
#define PI_DNS_H

#include "address.h"

namespace net {

bool QueryDNS(const char *address_to_query, Address *out_address);

} // namespace net

#endif // PI_DNS_H