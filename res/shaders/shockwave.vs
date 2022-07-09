#version 420 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 center;
layout(location=2) in float radius;
layout(location=3) in float scale;
layout(location=4) in float strength;

uniform mat4 view_projection;

out vec2 v_uv;
out vec2 v_position;
out vec2 v_center;
out float v_radius;
out float v_scale;
out float v_strength;

void main()
{
    gl_Position = view_projection * vec4(position, 0.0, 1.0);
    v_uv = (gl_Position.xy + 1) / 2;
    v_position = position;
    v_center = center;
    v_radius = radius;
    v_scale = scale;
    v_strength = strength;
}