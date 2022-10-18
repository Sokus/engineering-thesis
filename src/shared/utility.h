#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>

uint32_t SafeTruncateU64(uint64_t value);

uint32_t Popcount(unsigned int x);
uint32_t Log2(unsigned int x);
int BitsRequired(unsigned int value);
int BitsRequiredForRange(unsigned int min, unsigned int max);

#endif // BASE_TYPES_H