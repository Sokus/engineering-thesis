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
 * Counts the number of high bits in a 32-bit number
 */
unsigned int Popcount(unsigned int x)
{
	const unsigned int a = x - ((x >> 1) & 0x55555555);
	const unsigned int b = (((a >> 2) & 0x33333333) + (a & 0x33333333));
	const unsigned int c = (((b >> 4) + b) & 0x0f0f0f0f);
	const unsigned int d = c + (c >> 8);
	const unsigned int e = d + (d >> 16);
	const unsigned int result = e & 0x0000003f;
	return result;
}

/**
 * Base2 Logarithm
 */
unsigned int Log2(unsigned int x)
{
	const unsigned int a = x | (x >> 1);
	const unsigned int b = a | (a >> 2);
	const unsigned int c = b | (b >> 4);
	const unsigned int d = c | (c >> 8);
	const unsigned int e = d | (d >> 16);
	const unsigned int f = e >> 1;
	return Popcount(f);
}

/**
 * Get the minimum number of bits required to represent a number
 */
int BitsRequired(unsigned int value)
{
    return Log2(value) + 1;
}

/**
 * Get the minimum number of bits required to represent an offset in range [min, max]
 */
int BitsRequiredForRange(unsigned int min, unsigned int max)
{
    return (min == max) ? 0 : BitsRequired(max - min);
}
