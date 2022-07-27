#version 420 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 light_position;
layout(location=2) in vec3 light_color;
layout(location=3) in vec4 attenuation;

uniform mat4 view_projection;
uniform float brightness_multiplier;

out vec2 v_position;
out vec2 v_light_position;
out vec3 v_light_color;
out vec4 v_attenuation;

void main()
{
    gl_Position = view_projection * vec4(position, 0.0, 1.0);
    v_position = position;
    v_light_position = light_position;
    v_light_color = light_color * brightness_multiplier;
    v_attenuation = attenuation;
}