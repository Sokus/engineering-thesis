#include <stdint.h>
#include <stdio.h>

#include "serialize.h"
#include "bitpacker.h"
#include "log.h"

typedef struct vec2i
{
    int x, y;
} vec2i;

int main(int argc, char *argv[])
{
    bool ob1 = true;
    bool ob2 = false;
    bool ob3 = true;
    vec2i ov = { -1, 1 };

    char buffer[128];
    BitPacker bw = BitWriter(buffer, sizeof(buffer));
    SERIALIZE_BOOL(&bw, &ob1);
    SERIALIZE_BOOL(&bw, &ob2);
    SERIALIZE_BOOL(&bw, &ob3);
    SerializeInteger(&bw, &ov.x, -1, 1);
    SerializeInteger(&bw, &ov.y, -1, 1);
    Log(LOG_INFO, "bits used: %d", BitsWritten(&bw));
    Flush(&bw);

    bool ib1 = false;
    bool ib2 = false;
    bool ib3 = false;
    vec2i iv = {0};
    BitPacker br = BitReader(buffer, sizeof(buffer));
    SERIALIZE_BOOL(&br, &ib1);
    SERIALIZE_BOOL(&br, &ib2);
    SERIALIZE_BOOL(&br, &ib3);
    SerializeInteger(&br, &iv.x, -1, 1);
    SerializeInteger(&br, &iv.y, -1, 1);
    Log(LOG_INFO, "\n"
        "  bool(1): %d\n"
        "  bool(2): %d\n"
        "  bool(3): %d\n"
        "  vec2i: %d %d", ib1, ib2, ib3, iv.x, iv.y);

    return 0;
}