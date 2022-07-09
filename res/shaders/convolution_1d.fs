#version 420 core

in vec2 v_uv;

uniform sampler2D tex;
uniform float kernel[32];
uniform int kernel_length;
uniform vec2 sampling_step;

out vec4 frag_color;

void main()
{
    vec2 sampling_offset = ((-kernel_length / 2.0) + 0.5) * sampling_step;
    vec4 result = vec4(0.0);
    for(int i=0; i<kernel_length; ++i)
        result += kernel[i] * texture(tex, v_uv + sampling_offset + i*sampling_step);
    frag_color = result;
}