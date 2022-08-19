#include "config.h"

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #include <windns.h>
#elif defined(PLATFORM_LINUX)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

#include "dns.h"
#include "log.h"

namespace net {

unsigned int QueryDNS(const char *hostname)
{
#if defined(PLATFORM_WINDOWS)
    DNS_RECORDA *query_result = nullptr;
    DNS_STATUS dns_status = DnsQuery_A(hostname, DNS_TYPE_A, DNS_QUERY_STANDARD,
                                       0, &query_result, 0);
    if(dns_status == DNS_RCODE_NOERROR)
    {
        IP4_ADDRESS ip4_address = query_result->Data.A.IpAddress;
        unsigned int address = ntohl(ip4_address);
        return address;
    }
    else
    {
        CHAR buffer[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dns_status, 0, buffer, sizeof(buffer), 0);
        Log(LOG_ERROR, "QueryDNS failed: %s", buffer);
        return 0;
    }
#elif defined(PLATFORM_LINUX)
    struct addrinfo* addrinfo;
    int status = getaddrinfo(hostname, 0, 0, &addrinfo);
    if(status == 0)
    {
        struct sockaddr_in* sockaddr = (struct sockaddr_in*)addrinfo->ai_addr;
        unsigned int address = ntohl(sockaddr->sin_addr.s_addr);
        return address;
    }
    else
    {
        Log(LOG_ERROR, "QueryDNS failed: %s", gai_strerror(status));
        return 0;
    }
#endif
    return 0;
}

} // namespace net