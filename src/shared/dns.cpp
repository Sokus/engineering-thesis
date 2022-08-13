#include "config.h"

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #include <windns.h>
    #include <stdarg.h> // va_list, va_start(), va_end()
    // link against Dnsapi.lib
#elif defined(PLATFORM_LINUX)
#endif

#include "dns.h"
#include "log.h"

namespace net {

bool QueryDNS(const char *address_to_query, Address *out_address)
{
    if(out_address != nullptr)
    {
        DNS_RECORDA *query_result = nullptr;
        DNS_STATUS dns_status = DnsQuery_A(address_to_query, DNS_TYPE_A, DNS_QUERY_STANDARD, 0, &query_result, 0);

        if(dns_status == DNS_RCODE_NOERROR)
        {
            IP4_ADDRESS ip4_address = query_result->Data.A.IpAddress;
            out_address->SetAddress(ntohl(ip4_address));
            return true;
        }
        else
        {
            CHAR buffer[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dns_status, 0, buffer, 1024, 0);
            Log(LOG_ERROR, "QueryDNS failed: %s", buffer);
            return false;
        }
    }

    return false;
}


} // namespace net