#include <stdio.h>
#include <stdint.h>

// networking
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_addr
#include <fcntl.h>        // set file/socket handle to non-blocking
#include <unistd.h>       // write(), close()
#include <errno.h>        // socket error handling

#include "pi_helpers.h"
#include "pi_networking.h"
#include "pi_networking.cpp"
#include "pi_linux_networking.cpp"

int main(void)
{
    
    return 0;
}