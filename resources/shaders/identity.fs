#version 420 core

in vec2 v_uv;
uniform sampler2D tex;
out vec4 frag_color;

void main() {
    frag_color = texture(tex, v_uv);
}