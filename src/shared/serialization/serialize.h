#ifndef PI_SERIALIZE_H
#define PI_SERIALIZE_H

#include <stdint.h>

#include "bitpacker.h"
#include "macros.h"

bool SerializeInteger(BitPacker *bit_packer, int32_t *value, int32_t min, int32_t max);

#define SERIALIZE_BITS(bit_packer_ptr, type, value_ptr, bits)\
    do {\
        ASSERT((bits) > 0);\
        ASSERT((bits) <= 32);\
        if(WouldOverflow((bit_packer_ptr), (bits)))\
        { return false; }\
        uint32_t _u;\
        if((bit_packer_ptr)->mode == BIT_WRITER)\
        {\
            _u = (uint32_t)(*(value_ptr));\
            WriteBits((bit_packer_ptr), &_u, (bits));\
        }\
        if((bit_packer_ptr)->mode == BIT_READER)\
        {\
            ReadBits((bit_packer_ptr), &_u, (bits));\
            *value_ptr = (type)_u;\
        }\
    } while (0)

#define SERIALIZE_INT32(bit_packer_ptr, value_ptr) SERIALIZE_BITS((bit_packer_ptr), int32_t, (value_ptr), 32)
#define SERIALIZE_INT16(bit_packer_ptr, value_ptr) SERIALIZE_BITS((bit_packer_ptr), int16_t, (value_ptr), 16)
#define SERIALIZE_UINT32(bit_packer_ptr, value_ptr) SERIALIZE_BITS((bit_packer_ptr), uint32_t, (value_ptr), 32)
#define SERIALIZE_UINT16(bit_packer_ptr, value_ptr) SERIALIZE_BITS((bit_packer_ptr), uint16_t, (value_ptr), 16)
#define SERIALIZE_BOOL(bit_packer_ptr, value_ptr) SERIALIZE_BITS((bit_packer_ptr), bool, (value_ptr), 1)

#define SERIALIZE_INT(bit_packer_ptr, value_ptr, min, max)\
    do {\
        if(!SerializeInteger(bit_packer_ptr, value_ptr, min, max))\
            return false;\
    } while(0)

#endif // PI_SERIALIZE_H