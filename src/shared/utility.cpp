#include "utility.h"
#include "macros.h"

/**
 * Converts 64-bit unsigned int to 32-bit while making sure
 * there won't be any value change.
 */
unsigned int SafeTruncateU64(unsigned long long int value)
{
    ASSERT(value <= 0xFFFFFFFF);
    unsigned int result = (unsigned int)value;
    return result;
}
