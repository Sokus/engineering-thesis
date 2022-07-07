#version 420 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_tex_coord;
out vec2 uv;
uniform mat4 u_model_to_world;
uniform mat4 u_camera_to_clip;
void main()
{
    mat4 transformation = u_camera_to_clip * u_model_to_world;
    gl_Position = transformation * vec4(a_pos, 0.0f, 1.0f);
    //gl_Position += vec4(1.0f, 1.0f, 0.0f, 0.0f);
    uv = a_tex_coord;
}