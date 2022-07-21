/* date = July 2nd 2022 5:37 pm */

#ifndef PLATFORM_H
#define PLATFORM_H

// NOTE(sokus): We need enough space to hold all possible
// keycodes on a platform, for example SDL has 354 keycodes total.
#define INPUT_KEY_COUNT 512
typedef struct Input
{
    bool keys_down[INPUT_KEY_COUNT];
    float keys_down_duration[INPUT_KEY_COUNT];
    float keys_down_duration_previous[INPUT_KEY_COUNT];
} Input;

typedef struct Texture
{
    bool is_valid;
    unsigned int id;
    int width;
    int height;
    int channels;
    int tile_width;
    int tile_height;
    int tile_count_x;
    int tile_count_y;
    int tile_count;
} Texture;

#endif //PLATFORM_H
