unsigned int SafeTruncateU64(unsigned long long int value)
{
    ASSERT(value <= 0xFFFFFFFF);
    unsigned int result = (unsigned int)value;
    return result;
}

Input CreateInput(void)
{
    Input result = {0};
    for(int key_idx = 0;
        key_idx < ARRAY_SIZE(result.keys_down_duration);
        key_idx++)
    {
        result.keys_down_duration[key_idx] = -1.0f;
    }
    
    for(int key_idx = 0;
        key_idx < ARRAY_SIZE(result.keys_down_duration_previous);
        key_idx++)
    {
        result.keys_down_duration_previous[key_idx] = -1.0f;
    }
    return result;
}

void UpdateInput(Input *input, float dt)
{
    MEMORY_COPY(input->keys_down_duration_previous,
                input->keys_down_duration,
                sizeof(input->keys_down_duration));
    
    for(int key_idx = 0; key_idx < ARRAY_SIZE(input->keys_down); key_idx++)
    {
        float old_duration = input->keys_down_duration_previous[key_idx];
        bool is_down = input->keys_down[key_idx];
        bool was_down = old_duration >= 0.0f;
        float new_duration = -1.0f;
        if(is_down)
        {
            if(was_down)
                new_duration = old_duration + dt;
            else
                new_duration = 0.0f;
        }
        input->keys_down_duration[key_idx] = new_duration;
    }
}

bool IsDown(Input *input, int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = input->keys_down[key_index];
    return result;
}

bool WasDown(Input *input, int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = input->keys_down_duration_previous[key_index] >= 0.0f;
    return result;
}

bool Pressed(Input *input, int key_index)
{
    ASSERT(key_index >= 0 && key_index < ARRAY_SIZE(input->keys_down));
    bool result = IsDown(input, key_index) && !WasDown(input, key_index);
    return result;
}
