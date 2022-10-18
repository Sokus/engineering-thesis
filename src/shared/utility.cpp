#include "utility.h"
#include "macros.h"

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

/**
 * Get the minimum number of bits required to represent a number
 */
int BitsRequired(unsigned int value)
{
    int pow = 0;
    if(1U << 31 < value) return 32;
    while(1U << pow < value) pow++;
    return pow;
}

/**
 * Get the minimum number of bits required to represent an offset in range [min, max]
 */
int BitsRequiredForRange(unsigned int min, unsigned int max)
{
    return (min == max) ? 0 : BitsRequired(max - min);
}
