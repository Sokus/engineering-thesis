#include "utility.h"
#include "macros.h"

#include <stdio.h>

/**
 * Converts 64-bit unsigned int to 32-bit while making sure
 * there won't be any value change.
 */
uint32_t SafeTruncateU64(uint64_t value)
{
    ASSERT(value <= 0xFFFFFFFF);
    uint32_t result = (uint32_t)value;
    return result;
}