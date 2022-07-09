#version 420 core

in vec2 uv;
in vec2 light_map_uv;

uniform sampler2DArray u_textures;
uniform sampler2D light_map;
uniform int u_layer;
uniform bool u_flip;

out vec4 frag_color;

void main()
{
    vec2 uv_p = uv;
    if(u_flip)
        uv_p.x = uv.x * -1.0f + 1.0f;
    vec3 light = texture(light_map, light_map_uv).rgb;
    frag_color = texture(u_textures, vec3(uv_p, u_layer)) * vec4(light, 1);
    //frag_color = vec4(0.9f, 0.0f, 1.0f, 1.0f);
}