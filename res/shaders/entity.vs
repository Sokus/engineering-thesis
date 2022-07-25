#version 420 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture_coordinates;

uniform mat4 model_to_world_projection;
uniform mat4 camera_to_clip_projection;

out vec2 uv;
out vec2 light_map_uv;

void main()
{
    mat4 transformation = camera_to_clip_projection * model_to_world_projection;
    gl_Position = transformation * vec4(position, 0.0f, 1.0f);
    light_map_uv = (gl_Position.xy + 1) / 2;
    //gl_Position += vec4(1.0f, 1.0f, 0.0f, 0.0f);
    uv = texture_coordinates;
}