#version 420 core

in vec2 v_uv;

uniform sampler2D tex;
uniform vec3 threshold = vec3(0.33);

out vec4 frag_color;

void main() 
{
    frag_color = vec4(max(vec3(0), texture(tex, v_uv).rgb - threshold), 1);
}