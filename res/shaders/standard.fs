#version 420 core
out vec4 frag_color;
in vec2 uv;
uniform sampler2DArray u_textures;
uniform int u_layer;
uniform bool u_flip;
void main()
{
    vec2 uv_p = uv;
    if(u_flip)
        uv_p.x = uv.x * -1.0f + 1.0f;
    frag_color = texture(u_textures, vec3(uv_p, u_layer));
    //frag_color = vec4(0.9f, 0.0f, 1.0f, 1.0f);
}