#include <stdio.h>

#include "log.h"

int main(int argc, char *argv[])
{
    Log(LOG_INFO, "SERVER: Started");
    Log(LOG_INFO, "SERVER: Exiting");
    return 0;
}