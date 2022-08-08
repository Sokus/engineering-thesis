// Base vertex shader for postprocessing
#version 420 core

out vec2 v_uv;

void main() 
{
    // We just want to render a fullscreen quad,
    // so we hardcode the model into the shader for simplicity.
    vec2 corners[] = {
        vec2(-1,-1),
        vec2( 1,-1),
        vec2(-1, 1),
        vec2(-1, 1),
        vec2( 1,-1),
        vec2( 1, 1)
    };
    gl_Position = vec4(corners[gl_VertexID], 0, 1);
    v_uv = (corners[gl_VertexID]+1) / 2;
}