#version 420 core

in vec2 uv;
in vec2 light_map_uv;

uniform sampler2DArray texture_array;
uniform sampler2D light_map;
uniform int layer;
uniform bool flip;

out vec4 fragment_color;

void main()
{
    vec2 uv_p = uv;
    if(flip)
        uv_p.x = uv.x * -1.0f + 1.0f;
    vec3 light = texture(light_map, light_map_uv).rgb;
    vec4 texel = texture(texture_array, vec3(uv_p, u_layer))
    fragment_color = texel * vec4(light, 1);
    //fragment_color = vec4(0.9f, 0.0f, 1.0f, 1.0f);
}