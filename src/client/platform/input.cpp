#include "input.h"
#include "platform.h"

#include "base/base.h"

namespace OS {

void SDL2_Input::Init()
{
    MEMORY_SET(keys_down, false, ARRAY_SIZE(keys_down));

    for(int key_idx = 0;
        key_idx < ARRAY_SIZE(keys_down_duration);
        key_idx++)
    {
        keys_down_duration[key_idx] = -1.0f;
    }

    for(int key_idx = 0;
        key_idx < ARRAY_SIZE(keys_down_duration_previous);
        key_idx++)
    {
        keys_down_duration_previous[key_idx] = -1.0f;
    }
}

void SDL2_Input::Update(float dt)
{
    MEMORY_COPY(keys_down_duration_previous,
                keys_down_duration,
                sizeof(keys_down_duration));

    for(int key_idx = 0; key_idx < ARRAY_SIZE(keys_down); key_idx++)
    {
        float old_duration = keys_down_duration_previous[key_idx];
        bool is_down = keys_down[key_idx];
        bool was_down = old_duration >= 0.0f;
        float new_duration = -1.0f;
        if(is_down)
        {
            if(was_down)
                new_duration = old_duration + dt;
            else
                new_duration = 0.0f;
        }
        keys_down_duration[key_idx] = new_duration;
    }
}

void SDL2_Input::SetByKeycode(SDL_Keycode keycode, bool value)
{
    unsigned int index;
    index = (keycode <= 0x7F) ? keycode : (keycode + SDL2_KEYCODE_OFFSET);
    keys_down[index] = value;
}

bool IsKeyDown(int key_index)
{
    SDL2_Context *ctx = SDL2_GetContext();
    SDL2_Input *input = &ctx->input;

    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = input->keys_down[key_index];
    return result;
}

bool WasKeyDown(int key_index)
{
    SDL2_Context *ctx = SDL2_GetContext();
    SDL2_Input *input = &ctx->input;

    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = input->keys_down_duration_previous[key_index] >= 0.0f;
    return result;
}

bool IsKeyPressed(int key_index)
{
    SDL2_Context *ctx = SDL2_GetContext();
    SDL2_Input *input = &ctx->input;

    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = IsKeyDown(key_index) && !WasKeyDown(key_index);
    return result;
}


} // namespace