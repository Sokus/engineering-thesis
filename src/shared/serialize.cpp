#include <stdint.h>

#include "serialize.h"
#include "bitpacker.h"
#include "macros.h"
#include "utility.h"

bool SerializeInteger(BitPacker *bit_packer, int32_t *value, int32_t min, int32_t max)
{
    ASSERT(min <= max);
    int bits_required = BitsRequiredForRange((uint32_t)min, (uint32_t)max);
    if(WouldOverflow(bit_packer, bits_required))
    {
        return false;
    }

    uint32_t unsigned_value;
    if(bit_packer->mode == BIT_WRITER)
    {
        ASSERT(*value >= min);
        ASSERT(*value <= max);
        unsigned_value = (uint32_t)(*value - min);
        WriteBits(bit_packer, &unsigned_value, bits_required);
    }

    if(bit_packer->mode == BIT_READER)
    {
        ReadBits(bit_packer, &unsigned_value, bits_required);
        int32_t signed_value = (int32_t)unsigned_value + min;
        if(signed_value < min || signed_value > max)
        {
            return false;
        }
        *value = signed_value;
    }
    return true;
}
