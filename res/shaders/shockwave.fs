#version 420 core

in vec2 v_uv;
in vec2 v_position;
in vec2 v_center;
in float v_radius;
in float v_scale;
in float v_strength;

const float bias = exp(-4);

uniform sampler2D tex;

out vec4 frag_color;

void main()
{
    vec2 displacement = v_position - v_center;
    float dist_from_center = length(displacement);
    vec2 pixel_size = vec2(1)/vec2(textureSize(tex,0).xy);

    float arg = (dist_from_center - v_radius)/v_scale;
    float alpha = max(0, exp(-arg*arg) - bias);

    vec2 uv_offset = -normalize(displacement)*alpha*v_strength*pixel_size;

    frag_color = vec4(
        texture(tex, v_uv + uv_offset).rgb,
        //vec3(1,0.5,0),
        //vec3(uv_offset, 0.0),
        alpha
    );
}